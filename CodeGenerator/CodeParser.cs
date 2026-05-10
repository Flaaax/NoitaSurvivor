using System.Runtime.Serialization;
using System.Security.Cryptography.X509Certificates;
using System.Text.RegularExpressions;

namespace CodeGenerator {
	enum TokenType {
		Comment,
		Word,
		Symbol,
		Literal,
	}
	record struct Token(string Name, TokenType Type);
	record struct TokenPattern(string Pattern, TokenType Type);
	public readonly struct CppStructInfo(string StructName) {
		public readonly string Name = StructName;
		public readonly List<string> Fields = [];
	};
	public readonly struct CppEnumInfo(string enumName) {
		public readonly string Name = enumName;
		public readonly List<string> enums = [];
	}
	public class StopIterationException : Exception { }

	class TokenIterator(List<Token> tokens, int index = 0) {
		readonly List<Token> tokens = tokens;
		int i = index;

		public void Next() {
			if (++i >= tokens.Count) throw new StopIterationException();
		}
		public Token Current() {
			if (i >= tokens.Count) throw new StopIterationException();
			return tokens[i];
		}
		public Token? Last() {
			return i >= 1 ? tokens[i - 1] : null;
		}
		public void Find(string? name, TokenType? type = null) {
			while ((type != null && Current().Type != type) || (name != null && Current().Name != name)) Next();
		}
		//Skip current scope at {
		public void SkipScope() {      //Current should be {
			if (Current().Name != "{") {
				throw new InvalidOperationException($"Invalid function call here, with Current().Name = {Current().Name}");
			}
			int count = 1;
			while (count != 0) {
				Next();
				switch (Current().Name) {
					case "{":
						count++;
						break;
					case "}":
						count--;
						break;
				}
			}
			Next();
		}
		public void SkipField() {
			int count = 0;
			while (true) {
				switch (Current().Name) {
					case "{":
						count++;
						break;
					case "}":
						count--;
						break;
					case ";":
						if (count == 0) {
							Next();
							return;
						}
						break;
				}
				Next();
			}
		}
		public void SkipFunction() {
			int count = 0;
			bool hasBody = false;
			while ((!hasBody) || count != 0) {
				switch (Current().Name) {
					case "{":
						hasBody = true;
						count++;
						break;
					case "}":
						count--;
						break;
					case ";":
						if (!hasBody) {
							Next();
							return;
						}
						break;
				}
				Next();
			}
		}
	}
	internal class CodeParser {
		readonly static char[] skipChars = [' ', '\n', '\r', '\t'];
		readonly static TokenPattern[] tokenPatterns = [
			new (@"^//.*?(?=\r?\n|\r|$)", TokenType.Comment),					//单行注释
			new (@"^/\*[\s\S]*?\*/", TokenType.Comment),						//多行注释
			new (@"^""(?:\\.|[^""\\\r\n])*""", TokenType.Literal),				//字符串字面量（只支持简单版本)
			new (@"^'(?:\\.|[^'\\\r\n])'", TokenType.Literal),					//字符字面量
			new (@"^(?:\d+|(?:\d+\.|\d*\.\d+)f)", TokenType.Literal),			//数字字面量 （只支持整数，float）
			new (@"^[a-zA-Z_][a-zA-Z0-9_]*", TokenType.Word),					//关键字或标识符
			new (@"^[=+*/&^#!{}()\[\]<><?;:|,~:\\%.\-]", TokenType.Symbol)		//符号
		];
		static Match MatchFromIndex(string input, string pattern, int startIndex) {
			if (input == null)
				throw new ArgumentNullException(nameof(input), "原始字符串不能为 null");
			if (pattern == null)
				throw new ArgumentNullException(nameof(pattern), "正则模式不能为 null");
			if (startIndex < 0 || startIndex > input.Length)
				throw new ArgumentOutOfRangeException(nameof(startIndex), "起始下标必须在 0 到字符串长度之间");
			string subInput = input[startIndex..];
			if (string.IsNullOrEmpty(subInput))
				return Match.Empty;
			string strictPattern = pattern.StartsWith('^') ? pattern : "^" + pattern;
			//Console.WriteLine($"匹配中 {counter++}");
			return Regex.Match(
				input: subInput,
				pattern: strictPattern,
				options: RegexOptions.None
			);
		}

		public static List<Token> Parse(string code) {
			List<Token> tokens = [];
			int index = 0;
			while (index < code.Length) {
				while (skipChars.Contains(code[index])) {
					//Console.WriteLine($"跳过字符：{code[index]}");
					index++;
				}
				bool hasMatched = false;
				//Console.WriteLine($"到达字符：{code[index]}");
				foreach (var tp in tokenPatterns) {
					var match = MatchFromIndex(code, tp.Pattern, index);
					if (match.Success) {
						tokens.Add(new(match.Value, tp.Type));
						index += match.Length;
						hasMatched = true;
						break;
					}
				}
				if (!hasMatched) break;
			}

			return tokens;
		}

		//源代码不允许使用C风格数组
		public static List<CppStructInfo> GetCppStructs(List<Token> tokens) {
			List<CppStructInfo> structs = [];
			TokenIterator it = new(tokens);
			try {
				while (true) {
					it.Find("struct", TokenType.Word);
					if (it.Last()?.Name == "N_NOINIT") {
						it.Find("{");
						it.SkipScope();
						continue;
					}
					it.Next();
					string name = it.Current().Name;
					if (it.Current().Type == TokenType.Word && name == "N_NOINIT") {
						it.Next();     //Skip attr
						it.Next();     //Skip name
						it.SkipScope();
						continue;
					}
					//Skip if not a declaration
					if (it.Current().Name == ";") {
						it.Next();
						continue;
					}
					it.Next();     //Skip name
					it.Next();     //Skip {
								   //Inside the struct
					bool stop = false;
					bool canVisit = true;
					CppStructInfo info = new(name);
					Console.WriteLine($"找到struct {name}");
					while (!stop) {
						while (it.Current().Type == TokenType.Comment) {       //Skip comments
							it.Next();
						}
						//Console.WriteLine($"遍历Token：{Current().Name}");
						switch (it.Current().Name) {
							case "public":
								canVisit = true;
								it.Next();
								break;
							case "private" or "protected":
								canVisit = false;
								it.Next();
								break;
							case "{":
								it.SkipScope();
								break;
							case "}":       //End of struct
								stop = true;
								break;
							case "union" or "struct" or "class":
								it.Find("{");
								it.SkipScope();
								break;
							default:        //Is a struct member
											//Console.WriteLine("找到可能的字段");
								bool shouldSkip = it.Current().Name is "const" or "volatile" or "N_NOINIT"
									|| (!canVisit);
								Token last = it.Current();
								it.Next();
								bool isField = false;
								while (true) {
									if (it.Current().Name == ";") {
										isField = true;
										break;
									}
									if (it.Current().Name == "(") {
										break;
									}
									if (it.Current().Name is "{" or "=") {
										isField = true;
										break;
									}
									last = it.Current();
									it.Next();
								}
								if (isField) {
									if (!shouldSkip) info.Fields.Add(last.Name);
									it.SkipField();
									//Console.WriteLine($"加入字段 {last.Name}");
								}
								else {
									it.SkipFunction();
									//Console.WriteLine("不是一个字段");
									//Console.WriteLine($"当前Token： {Current().Name}");
								}
								break;
						}
					}
					structs.Add(info);
				}
			}
			catch (StopIterationException) { }
			return structs;
		}

		public static void TestMain() {
			string code = """
								#pragma once
				#include"src/game/Contact/ContactLayerRules.h"
				#include"src/utils/Timer.h"
				#include"src/utils/Vec2.h"
				#include"src/ecs/types.h"


				struct EntityComponent {
					ContactLayer layer = ContactLayer::None;

					int health = -1;

					void takeDamage(int damage) {
						if (health <= 0)return;
						health -= damage;
						if (health <= 0) {
							health = 0;
						}
					}

					void kill() {
						health = 0;
					}

					bool isAlive()const {
						return health != 0;
					}
				};


				struct MaterialComponent {
					int value = 1;
				};


				struct LifeTimeComponent {
					Timer lifeTimer;
				};

				//need EntityComponent, BodyComponent
				//struct ContactPrefComponent {
				//	enum : char {
				//		NoPref = 0,
				//		Enabled = 2,
				//		Disabled = 1,
				//	};
				//
				//	std::array<char, ContactManager::max_type_size> contactPref = {};	//init as NoPref
				//
				//	void setContactPref(ContactLayer t, bool enabled) {
				//		contactPref[t] = enabled ? Enabled : Disabled;
				//	}
				//
				//};

				class b2Contact;
				class SpellBlock;
				class ProjectileSpell;
				class ModifierSpell;

				//need BodyComponent, EntityComponent
				struct ProjectileComponent {
					int damage = 0;
					float impulse = 0;
					float maxSpeed = 35.f;

					int pierce = 0;
					nvec2 impulseDir;		//impulse dir recieved
					nvec2 impulsePosFix;
					std::shared_ptr<SpellBlock> spellOnDeath;
					std::shared_ptr<ProjectileSpell> spell;			//todo change all these pointers to raw pointer
					std::vector<std::shared_ptr<ModifierSpell>> mods;
					//should be called in contact listener
					void onContact(myecs::entity self, myecs::entity other, b2Contact* contact);

					bool canPierce() const {
						return pierce != 0;
					}

					void doPierce() {
						if (pierce < 0)return;
						if (pierce > 0)pierce--;
					}

					void onDeath(myecs::entity self) const;

					std::shared_ptr<SpellBlock> getSpellOnDeath();
				};


				struct EnemyComponent {
					void updateDirection(myecs::entity self);
				};


				struct DirectionControlComponent {
					//bool enabled = true;
				};


				//for piercing entites and explosions
				//maybe use std pmr (so I think it can be 10x faster)
				struct MultiContactComponent {
					float contact_interval = 0.1f;
					std::unordered_map<myecs::entity, Timer> disabledContacts;

					void addContact(myecs::entity e, float interval) {
						disabledContacts[e].set(interval).start();
					}

					void addContact(myecs::entity e) {
						disabledContacts[e].set(contact_interval).start();
					}

					void update(float deltaTime) {
						std::vector<myecs::entity> expired;
						for (auto& it : disabledContacts) {
							it.second.update(deltaTime);
							if (!it.second.isRunning()) {
								expired.emplace_back(it.first);
							}
						}
						for (auto& e : expired) {
							disabledContacts.erase(e);
						}
					}

					bool isBannedContact(myecs::entity e) const {
						auto it = disabledContacts.find(e);
						return it != disabledContacts.end();
					}
				};
				""";
			try {
				// 2. 解析为Token列表
				Console.WriteLine("===== 开始解析Token =====");
				List<Token> tokens = CodeParser.Parse(code);
				Console.WriteLine($"Token解析完成，共生成 {tokens.Count} 个Token");
				Console.WriteLine();

				// 3. 提取结构体信息
				Console.WriteLine("===== 提取结构体信息 =====");
				List<CppStructInfo> structs = CodeParser.GetCppStructs(tokens);

				// 4. 打印结构体信息
				if (structs.Count == 0) {
					Console.WriteLine("未找到有效结构体");
				}
				else {
					foreach (var structInfo in structs) {
						// 打印结构体名称
						Console.WriteLine($"结构体名称：{structInfo.Name}");
						// 打印字段列表
						Console.WriteLine("字段列表：");
						if (structInfo.Fields.Count == 0) {
							Console.WriteLine("  无字段");
						}
						else {
							foreach (var field in structInfo.Fields) {
								Console.WriteLine($"  - {field}");
							}
						}
						Console.WriteLine("------------------------");
					}
				}
			}
			catch (Exception ex) {
				Console.WriteLine($"程序执行异常：{ex.Message}");
				Console.WriteLine($"异常详情：{ex.StackTrace}");
			}
		}
	}
}
