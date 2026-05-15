#pragma once
#include "../Assert.h"
#include "../Pointer.h"
#include <memory>
#include <type_traits>

namespace Util {
	template <class Base, class Deleter>
	class IntrusiveList;

	struct IntrusiveNode {
	private:
		template <class Base, class Deleter>
		friend class IntrusiveList;
		void* prev;
		void* next;
	};

	// Polymorphic intrusive list
	// Allows user to define deleter or let it do nothing.
	template <class Base, class Deleter = std::default_delete<Base>>
	class IntrusiveList {
		static_assert(std::is_base_of_v<IntrusiveNode, Base>, "Base should derive IntrusiveNode");

	private:
		Base* head{};
		Base* tail{};
		size_t m_size{};
		Deleter deleter{};
		void try_delete(Base* node) {
			next(node) = {};
			prev(node) = {};
			if constexpr (std::is_invocable_v<Deleter, Base*>) {
				deleter(node);
			}
		}

		static Base*& next(Base* n) {
			return *reinterpret_cast<Base**>(&static_cast<IntrusiveNode*>(n)->next);
		}
		static Base*& prev(Base* n) {
			return *reinterpret_cast<Base**>(&static_cast<IntrusiveNode*>(n)->prev);
		}
		static const Base* next(const Base* n) {
			return reinterpret_cast<const Base*>(static_cast<const IntrusiveNode*>(n)->next);
		}
		static const Base* prev(const Base* n) {
			return reinterpret_cast<const Base*>(static_cast<const IntrusiveNode*>(n)->prev);
		}

		template <bool is_const>
		struct m_iterator {
			using pointer = std::conditional_t<is_const, const Base*, Base*>;
			pointer node;

			m_iterator(pointer node = {}) : node(node) {
			}
			bool operator==(const m_iterator&) const = default;
			operator bool() const {
				return bool(node);
			}
			Base* operator*()
				requires(!is_const)
			{
				return node;
			}
			Base* operator->()
				requires(!is_const)
			{
				return node;
			}
			const Base* operator*() {
				return node;
			}
			const Base* operator->() const {
				return node;
			}
			m_iterator& operator++() {
				node = next(node);
				return *this;
			}
			m_iterator& operator--() {
				node = prev(node);
				return *this;
			}
			m_iterator operator++(int) {
				m_iterator t(*this);
				return ++*this, t;
			}
			m_iterator operator--(int) {
				m_iterator t(*this);
				return --*this, t;
			}
			operator pointer() const {
				return get();
			}

			pointer get() const {
				return node;
			}
		};

		using iterator = m_iterator<false>;
		using const_iterator = m_iterator<true>;

	public:
		IntrusiveList() {
		}
		IntrusiveList(Deleter deleter) : deleter(deleter) {
		}
		IntrusiveList(IntrusiveList&& other) noexcept : head(std::move(other.head)),
														tail(std::move(other.tail)),
														m_size(std::move(other.m_size)),
														deleter(other.deleter) {
			other.head = {};
			other.tail = {};
			other.m_size = 0;
		}
		IntrusiveList(const IntrusiveList&) = delete;
		~IntrusiveList() {
			clear();
		}

		Base* emplace_back(Base* elem) {
			assertNotNull(elem);
			m_size++;
			if (!head) {
				head = elem;
				tail = elem;
				next(elem) = {};
				prev(elem) = {};
			} else {
				next(tail) = elem;
				prev(elem) = tail;
				tail = elem;
				next(tail) = {};
			}
			return elem;
		}

		Base* emplace_front(Base* elem) {
			assertNotNull(elem);
			m_size++;
			if (!head) {
				head = elem;
				tail = elem;
				next(elem) = {};
				prev(elem) = {};
			} else {
				prev(head) = elem;
				next(elem) = head;
				head = elem;
				prev(head) = {};
			}
			return elem;
		}

		Base* emplace_after(Base* cur, Base* elem) {
			assertNotNull(elem);
			assertNotNull(cur);
			m_size++;
			Base* next_node = next(cur);
			prev(elem) = cur;
			next(elem) = next_node;
			next(cur) = elem;
			if (next_node) {
				prev(next_node) = elem;
			} else if (cur == tail) {
				tail = elem;
			}
			return elem;
		}

		Base* emplace_before(Base* cur, Base* elem) {
			assertNotNull(elem);
			assertNotNull(cur);
			m_size++;
			Base* prev_node = prev(cur);
			next(elem) = cur;
			prev(elem) = prev_node;
			prev(cur) = elem;
			if (prev_node) {
				next(prev_node) = elem;
			} else if (cur == head) {
				head = elem;
			}
			return elem;
		}

		void pop_front() {
			if (!head) {
				return;
			}
			Base* old_head = head;
			head = next(head);
			if (head)
				prev(head) = {};
			else
				tail = {};
			try_delete(old_head);
			m_size--;
		}
		void pop_back() {
			if (!head) {
				return;
			}
			Base* old_tail = tail;
			tail = prev(tail);
			if (tail)
				next(tail) = {};
			else
				head = {};
			try_delete(old_tail);
			m_size--;
		}
		Base* front() {
			return head;
		}
		const Base* front() const {
			return head;
		}
		Base* back() {
			return tail;
		}
		const Base* back() const {
			return tail;
		}

		size_t size() const {
			return m_size;
		}
		bool empty() const {
			return m_size == 0;
		}

		iterator begin() {
			return iterator{head};
		}
		const_iterator begin() const {
			return const_iterator{head};
		}
		iterator end() {
			return {};
		}
		const_iterator end() const {
			return {};
		}

		iterator erase(iterator it) {
			if (!it) {
				return {};
			}
			if (m_size == 1) {
				m_size--;
				try_delete(it.node);
				head = {};
				tail = {};
				return {};
			}
			if (it.node == head) {
				Base* ret = next(head);
				pop_front();
				return {ret};
			}
			if (it.node == tail) {
				pop_back();
				return {};
			}

			Base* current = it.node;
			Base* prev_node = prev(current);
			Base* next_node = next(current);

			next(prev_node) = next_node;
			prev(next_node) = prev_node;

			try_delete(current);
			m_size--;

			return iterator{next_node};
		}

		iterator erase(iterator first, iterator last) {
			for (auto it = first; it != last;) {
				it = erase(it);
			}
			return last;
		}

		iterator insert(iterator it, Base* elem) {
			if (!it || !head || it.node == tail) {
				emplace_back(elem);
				return {tail};
			}
			if (it.node == head) {
				emplace_front(elem);
				return {head};
			}
			Base* cur = it.node;
			prev(next(cur)) = elem;
			next(elem) = next(cur);
			prev(elem) = cur;
			next(cur) = elem;
			return {elem};
		}

		void clear() {
			while (head) {
				Base* cur = head;
				head = next(head);
				try_delete(cur);
			}
			tail = {};
			m_size = 0;
		}

		void swap(IntrusiveList& other) {
			std::swap(head, other.head);
			std::swap(tail, other.tail);
			std::swap(m_size, other.m_size);
			std::swap(deleter, other.deleter);
		}

		bool contains(Base* node) {
			for (auto it = begin(); it != end; ++it) {
				if (node == *it)
					return true;
			}
			return false;
		}
	};

	struct NoDelete {};
	struct StringViewHash : public std::hash<std::string_view> {
		using is_transparent = void;
	};

	template <typename T>
	using BaseStdMap = std::unordered_map<
		std::string,
		T,
		StringViewHash,
		std::equal_to<> // C++14+
		>;

	template <class T>
	class StdMap : public BaseStdMap<T> {
		using Base = BaseStdMap<T>;

	public:
		T& operator[](std::string_view key) {
			if (auto it = this->find(key); it != this->end()) {
				return it->second;
			}
			return Base::operator[](std::string(key));
		}

		T* try_find(std::string_view key) {
			const auto it = Base::find(key);
			return it != Base::end() ? &it->second : nullptr;
		}
	};

	template <class T>
	struct Wrapper {
		T value;
		Wrapper() = default;
		Wrapper(const T& val) : value(val) {
		}
		Wrapper(T&& val) : value(std::move(val)) {
		}
		operator T&() {
			return value;
		}
		operator const T&() const {
			return value;
		}
	};

	using BoolWrapper = Wrapper<bool>;
} // namespace Util