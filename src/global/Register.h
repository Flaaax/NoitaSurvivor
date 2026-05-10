#pragma once
#include<vector>
#include<functional>
#include<memory>
#include<concepts>
#include<type_traits>


template<class Base>
class Register final :public Singleton<Register<Base>> {
	N_SINGLETON(Register<Base>)
private:
	using creator_t = std::function<n_shared<Base>()>;
	using Singleton = Singleton<Register<Base>>;

	struct TypeInfo {
		const size_t id;
		std::string typeName;
		creator_t creator;

		TypeInfo() :id(Singleton::inst().typeCount++) {
			Singleton::inst().types.push_back(this);
		}
		//TypeInfo(TypeInfo&&) = delete;
	};

	size_t typeCount = 0;
	std::vector<TypeInfo*> types;

	~Register() {
		for (auto* type : types) {
			delete type;
		}
	}
public:

	template<class T>
	static const size_t getId() {
		return getTypeInfo<T>().id;
	}

	template<class T>
	static const TypeInfo& getTypeInfo() {
		static TypeInfo* info = new TypeInfo();
		return *info;
	}

	template<class T>
	static const TypeInfo& getTypeInfo(const std::string& typeName, bool enable_creator) {
		auto& info = const_cast<TypeInfo&>(getTypeInfo<T>());
		info.typeName = typeName;
		if constexpr (std::is_base_of<Base, T>::value) {
			info.creator = enable_creator ? [] {return std::make_shared<T>(); } : nullptr;
		}
		else {
			info.creator = nullptr;
		}
		return info;
	}

	static const TypeInfo& getTypeInfo(const size_t id) {
		if (id >= Singleton::inst().typeCount) {
			MYASSERT(false, "unregistered id");
			throw std::runtime_error("unregistered id");
		}
		return *(Singleton::inst().types[id]);
	}

	static const size_t getIdCount() {
		return Singleton::inst().typeCount;
	}

	static const creator_t& getCreator(size_t typeId) {
		return getTypeInfo(typeId).creator;
	}
};