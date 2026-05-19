#pragma once
#include <cstddef>

namespace myecs {
	struct VariantTraits {
		bool move = true;
		// bool move_assign = true;
		bool copy = false;
		u64 align = alignof(std::max_align_t);

		template <class T>
		constexpr bool valid() const {
			return ((!move) || std::is_move_constructible_v<T>)
				//&& ((!move_assign) || std::is_move_assignable_v<T>)
				&&((!copy) || std::is_copy_constructible_v<T>);
		}
	};

	namespace detail {
		template <class T>
		static T& raw(void* _data) {
			return *static_cast<T*>(_data);
		}

		struct VariantVTable {
			using destroy_func_t = void (*)(void* _data);
			using assign_func_t = void (*)(void* _from, void* _to);
			destroy_func_t destroy{};
			assign_func_t move_construct{};
			// assign_func_t move_assign{};
			assign_func_t copy_construct{};

			template <class T>
			explicit VariantVTable(std::in_place_type_t<T>) {
				destroy = [](void* _data) { raw<T>(_data).~T(); };
				if constexpr (std::is_move_constructible_v<T>) {
					move_construct = [](void* _from, void* _to) { new (_to) T(std::move(raw<T>(_from))); };
				}
				/*if constexpr (std::is_move_assignable_v<T>) {
					move_assign = [](void* _from, void* _to) {raw<T>(_to) = std::move(raw<T>(_from)); };
				}*/
				if constexpr (std::is_copy_constructible_v<T>) {
					copy_construct = [](void* _from, void* _to) { new (_to) T(raw<T>(_from)); };
				}
			}

			template <class T>
			static VariantVTable* get() {
				static VariantVTable _table(std::in_place_type<T>);
				return &_table;
			}
		};
	} // namespace detail

	template <u64 MaxSize, VariantTraits Traits = VariantTraits()>
	class Variant {
	private:
		alignas(Traits.align) unsigned char data[MaxSize]{};
		detail::VariantVTable* table{};

		template <class T>
		T& raw() {
			return detail::raw<T>(static_cast<void*>(data));
		}

		template <class T>
		const T& raw() const {
			return detail::raw<T>(static_cast<void*>(data));
		}

		template <class T>
		static constexpr bool _valid =
			sizeof(T) <= MaxSize && alignof(T) <= Traits.align && Traits.valid<T>();

	public:
		Variant() noexcept {}

		Variant(Variant&& other) noexcept
			requires(Traits.move)
		{
			if (other.empty())
				return;
			table = other.table;
			table->move_construct(other.data, data);
			other.clear();
		}

		Variant(const Variant& other)
			requires(Traits.copy)
		{
			if (other.empty())
				return;
			table = other.table;
			table->copy_construct(other.data, data);
		}

		Variant& operator=(Variant&& other) noexcept
			requires(Traits.move)
		{
			if (this == &other || other.empty())
				return *this;
			clear();
			table = other.table;
			table->move_construct(other.data, data);
			other.clear();
			return *this;
		}

		~Variant() {
			clear();
		}

		void clear() {
			if (table) {
				table->destroy(data);
				table = {};
			}
		}

		template <class T, class... Args>
		T& emplace(Args&&... args)
			requires _valid<T>
		{
			clear();
			table = detail::VariantVTable::get<T>();
			return *std::launder(new (data) T(std::forward<Args>(args)...));
		}

		template <class T>
		bool has() const noexcept
			requires _valid<T>
		{
			return table == detail::VariantVTable::get<T>();
		}

		template <class T>
		T* try_get() noexcept
			requires _valid<T>
		{
			return has<T>() ? &raw<T>() : nullptr;
		}

		template <class T>
		const T* try_get() const noexcept
			requires _valid<T>
		{
			return has<T>() ? &raw<T>() : nullptr;
		}

		template <class T>
		T& get()
			requires _valid<T>
		{
			T* ptr = try_get<T>();
			if (!ptr)
				throw std::bad_variant_access();
			return *ptr;
		}

		template <class T>
		const T& get() const
			requires _valid<T>
		{
			const T* ptr = try_get<T>();
			if (!ptr)
				throw std::bad_variant_access();
			return *ptr;
		}

		template <class T>
		static constexpr bool valid() {
			return _valid<T>;
		}

		bool empty() const noexcept {
			return !table;
		}

		void* pointer() {
			return data;
		}

		const void* pointer() const {
			return data;
		}
	};

} // namespace myecs