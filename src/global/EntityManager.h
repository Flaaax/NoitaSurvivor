//#pragma once
//#include"src/game/Entities/EntityPool.h"
//#include"src/utils/Math.h"
//#include <unordered_map>
//#include<vector>
//
//
//class EntityManager :public Singleton<EntityManager> {
//private:
//	friend class Singleton<EntityManager>;
//	friend class Game;
//
//	constexpr static size_t MAX_ENTITY_COUNT = EntityPool::maxEntityCount;
//	std::vector<EntityPool> pools;
//	size_t entityTypeCount = 0;
//
//	std::unordered_map<size_t, std::shared_ptr<Entity>> allocatedEntities;		//should be tested
//
//	bool initialized = false;
//
//	NAsyncLogger aaaaalogger = LoggerFactory::createSync("EntityManager");
//
//	~EntityManager() {
//		for (const auto& pair : allocatedEntities) {
//			aaaaalogger->debug("activated entity id: {}, type: {}", pair.first, pair.second->getTypeName());
//		}
//	}
//
//	static void activateEntity(const n_shared<Entity>& e) {
//		EntityPool::activateEntity(e);
//	}
//
//public:
//	void init() {
//		if (initialized) {
//			MYASSERT(true, "EntityManager is already initialized!");
//			throw std::runtime_error("EntityManager is already initialized!");
//		}
//		entityTypeCount = Register<Entity>::getIdCount();
//		for (size_t i = 0; i < entityTypeCount; i++) {
//			pools.emplace_back(i);
//		}
//		initialized = true;
//	}
//
//	template<Entity_t T>
//	n_shared<T> getEntity(bool activate = false) {
//		size_t typeId = Register<Entity>::getId<T>();
//		auto ptr = getEntity(typeId);
//		if (auto ret_ptr = std::dynamic_pointer_cast<T>(ptr)) {
//			//allocatedEntities.insert({ ptr->entityId, ptr });
//			allocatedEntities[ptr->entityId] = ptr;
//			if (activate) {
//				EntityPool::activateEntity(ptr);
//			}
//			return ret_ptr;
//		}
//		else {
//			//this should never happen, basically
//			MYASSERT(true, "invalid entity type cast");
//			pools[typeId].returnEntity(ptr);
//			return nullptr;
//		}
//	}
//
//	n_shared<Entity> getEntity(size_t typeId) {
//		if (typeId >= entityTypeCount) {
//			MYASSERT(true, "trying to access an unregistered type");
//			return nullptr;
//		}
//		auto& pool = pools[typeId];
//		if (auto ptr = pool.getEntity()) {
//			return ptr;
//		}
//		MYASSERT(true, "invalid entity get");
//		return nullptr;
//	}
//
//	bool returnEntity(n_shared<Entity> e) {
//		//std::cout << "trying to return entity\n";
//		if (!e) {
//			MYASSERT(false, "trying to return nullptr");
//			Logger::warn("returning nullptr");
//			return false;
//		}
//		size_t typeId = e->getTypeId();
//		if (typeId >= entityTypeCount) {
//			MYASSERT(false, "trying to return unregistered type");
//			Logger::warn("returning unregistered type");
//			return false;
//		}
//		if (auto it = allocatedEntities.find(e->entityId); it != allocatedEntities.end()) {
//			auto& pool = pools[typeId];
//			pool.returnEntity(e);
//			allocatedEntities.erase(e->entityId);
//			//std::cout << "entity returned\n";
//			return true;
//		}
//		MYASSERT(false, "invalid entity id");
//		Logger::warn("invalid entity id");
//		return false;
//	}
//
//	n_shared<Entity> queryActivatedEntity(size_t entityId) {
//		if (auto it = allocatedEntities.find(entityId); it != allocatedEntities.end() && it->second->isActivated()) {
//			return it->second;
//		}
//		return nullptr;
//	}
//
//	/*std::unordered_map<size_t, std::shared_ptr<Entity>>& getActivatedEntities() {
//		return allocatedEntities;
//	}*/
//
//	void forEachActivatedEntity(const std::function<void(n_shared<Entity>&)>& func) {
//		MYASSERT((bool)(func), "func is invalid");
//		for (auto& entity : allocatedEntities) {
//			if (entity.second->isActivated()) {
//				func(entity.second);
//			}
//		}
//	}
//};
//
