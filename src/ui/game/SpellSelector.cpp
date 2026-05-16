//#include"SpellSelector.h"
//#include"../NWidget.h"
//#include"../NButton.h"
//#include"../NText.h"
//
//
//NSpellSelector::NSpellSelector(const nvec2& center, int selectCount, NWidget* parent) {
//	
//}
//
//NSpellSelector::~NSpellSelector() {
//	getParent()->destroy(editor);
//	getParent()->destroy(ui);
//}
//
//NSpellSelector* NSpellSelector::create(const nvec2& center, int selectCount) {
//	this->setParent(parent);
//	this->selectCount = selectCount;
//	m_geometry.size = { 350,250 };
//	m_geometry.setCenter(center);
//
//	float slotSize = NSlot::size;
//	float editorLength = selectCount * (NSlot::size - NSlot::outLine) + NSlot::outLine;
//	float left = m_geometry.x + (m_geometry.w - editorLength) / 2.f;
//
//	inventory.resize(selectCount);
//
//	editor = new NSpellInventory(parent, inventory, { left,m_geometry.y + m_geometry.h / 2.f - 25 });
//
//	ui = new NWidget(false);
//	ui->setParent(parent);
//
//	nrect buttonRect;
//	buttonRect.size = { 100,50 };
//	buttonRect.setCenter(m_geometry.center());
//	buttonRect.y += 70;
//	NButton* cancelButton = new NButton(buttonRect);
//	cancelButton->setOnClick([&] {
//		this->isRunning = false;
//	});
//	cancelButton->setParent(parent);
//	cancelButton->setText("取消");
//	cancelButton->setParent(ui);
//	cancelButton->getText().setCharacterSize(18);
//
//	nrect textRect(m_geometry.center(), { 10,10 }, 1);
//	textRect.y -= 65;
//	NText* text = new NText("占位符", NText::Center, 25U);
//	text->setGeometry(textRect);
//	text->setParent(ui);
//}
//
//void NSpellSelector::draw(Renderer& renderer) const {
//	sf::RectangleShape shape;
//	shape.setFillColor({ 200,200,200 });
//	shape.setOutlineColor({});
//	shape.setOutlineThickness(2.f);
//	shape.setPosition(m_geometry.position);
//	shape.setSize(m_geometry.size);
//	renderer.drawGui(shape);
//}
