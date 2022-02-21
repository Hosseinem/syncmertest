// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \author Hossein Eizadi Moghadam <hosseinem AT fu-berlin.de>
 * \brief Provides opensyncmer.
 */

#pragma once

#include <seqan3/std/algorithm>
#include <deque>

#include <seqan3/core/detail/empty_type.hpp>
#include <seqan3/core/range/detail/adaptor_from_functor.hpp>
#include <seqan3/core/range/type_traits.hpp>
#include <seqan3/utility/range/concept.hpp>
#include <seqan3/utility/type_traits/lazy_conditional.hpp>

namespace seqan3::detail
{
// ---------------------------------------------------------------------------------------------------------------------
// opensyncmer_view class
// ---------------------------------------------------------------------------------------------------------------------

/*!\brief The type returned by opensyncmer.
 * \tparam urng1_t The type of the underlying range, must model std::ranges::forward_range, the reference type must
 *                 model std::totally_ordered. The typical use case is that the reference type is the result of
 *                 seqan3::kmer_hash.
 * \implements std::ranges::view
 * \ingroup search_views
 *
 *
 * \note Most members of this class are generated by std::ranges::view_interface which is not yet documented here.

 */
template <std::ranges::view urng1_t, std::ranges::view urng2_t>
class opensyncmer_view : public std::ranges::view_interface<opensyncmer_view<urng1_t, urng2_t>>
{
private:
    static_assert(std::ranges::forward_range<urng1_t>, "The opensyncmer_view only works on forward_ranges.");
    static_assert(std::ranges::forward_range<urng2_t>, "The opensyncmer_view only works on forward_ranges.");
    static_assert(std::totally_ordered<std::ranges::range_reference_t<urng1_t>>,
                  "The reference type of the underlying range must model std::totally_ordered.");
    static_assert(std::totally_ordered<std::ranges::range_reference_t<urng2_t>>,
                  "The reference type of the underlying range must model std::totally_ordered.");

    //!\brief Whether the given ranges are const_iterable.
    static constexpr bool const_iterable = seqan3::const_iterable_range<urng1_t>;
    static constexpr bool const_iterable2 = seqan3::const_iterable_range<urng2_t>;

    //!\brief The first underlying range.
    urng1_t urange1{};
    //!\brief The second underlying range.
    urng2_t urange2{};
    //!\brief The size of k-mer.
    size_t K{};
    //!\brief The size of s-mer.
    size_t S{};

    template <bool const_range>
    class basic_iterator;

    //!\brief The sentinel type of the opensyncmer_view.
    using sentinel = std::default_sentinel_t;

public:
    /*!\name Constructors, destructor andt assignment
     * \{
     */
     /// \cond Workaround_Doxygen
    opensyncmer_view() requires  std::default_initializable<urng1_t> && std::default_initializable<urng2_t> = default; //!< Defaulted.
    /// \endcond
    opensyncmer_view(opensyncmer_view const & rhs) = default; //!< Defaulted.
    opensyncmer_view(opensyncmer_view && rhs) = default; //!< Defaulted.
    opensyncmer_view & operator=(opensyncmer_view const & rhs) = default; //!< Defaulted.
    opensyncmer_view & operator=(opensyncmer_view && rhs) = default; //!< Defaulted.
    ~opensyncmer_view() = default; //!< Defaulted.

    /*!\brief Construct from a view and a given number of values in one window.
    * \param[in] urange1     The input range to process. Must model std::ranges::viewable_range and
    *                        std::ranges::forward_range.
    * \param[in] urange2     The input range to process. Must model std::ranges::viewable_range and
    *                        std::ranges::forward_range.
    * \param[in] K The k-mer size used.
    * \param[in] S The s-mer size used.
    */
    opensyncmer_view(urng1_t urange1, urng2_t urange2, size_t const K, size_t const S) :
        urange1{std::move(urange1)},
        urange2{std::move(urange2)},
        K{K},
        S{S}
    {}

    /*!\brief Construct from a non-view that can be view-wrapped and a given number of values in one window.
    * \tparam other_urng1_t  The type of another urange. Must model std::ranges::viewable_range and be constructible from urng1_t.
    * \tparam other_urng2_t  The type of another urange. Must model std::ranges::viewable_range and be constructible from urng2_t.
    * \param[in] K The k-mer size used.
    * \param[in] S The s-mer size used.
    */
    template <typename other_urng1_t, typename other_urng2_t>
    //!\cond
        requires (std::ranges::viewable_range<other_urng1_t> &&
                  std::constructible_from<urng1_t, ranges::ref_view<std::remove_reference_t<other_urng1_t>>> &&
                  std::ranges::viewable_range<other_urng2_t> &&
                            std::constructible_from<urng2_t, ranges::ref_view<std::remove_reference_t<other_urng2_t>>>)
    //!\endcond
    opensyncmer_view(other_urng1_t && urange1, other_urng2_t && urange2, size_t const K, size_t const S) :
        urange1{std::views::all(std::forward<other_urng1_t>(urange1))},
        urange2{std::views::all(std::forward<other_urng2_t>(urange2))},
        K{K},
        S{S}
    {}

    /*!\name Iterators
     * \{
     */
    /*!\brief Returns an iterator to the first element of the range.
     * \returns Iterator to the first element.
     *
     * \details
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * Strong exception guarantee.
     */
    basic_iterator<false> begin()
    {
        return {std::ranges::begin(urange1),
                std::ranges::begin(urange2),
                std::ranges::end(urange1),
                K,
                S};
    }

    //!\copydoc begin()
    basic_iterator<true> begin() const
    //!\cond
        requires const_iterable && const_iterable2
    //!\endcond
    {
        return {std::ranges::cbegin(urange1),
                std::ranges::cbegin(urange2),
                std::ranges::cend(urange1),
                K,
                S};
    }

    /*!\brief Returns an iterator to the element following the last element of the range.
     * \returns Iterator to the end.
     *
     * \details
     *
     * This element acts as a placeholder; attempting to dereference it results in undefined behaviour.
     *
     * ### Complexity
     *
     * Constant.
     *
     * ### Exceptions
     *
     * No-throw guarantee.
     */
    sentinel end() const
    {
        return {};
    }
    //!\}
};

//!\brief Iterator for calculating opensyncmers.
template <std::ranges::view urng1_t, std::ranges::view urng2_t>
template <bool const_range>
class opensyncmer_view<urng1_t, urng2_t>::basic_iterator
{
private:
    //!\brief The sentinel type of the first underlying range.
    using urng1_sentinel_t = maybe_const_sentinel_t<const_range, urng1_t>;
    //!\brief The iterator type of the first underlying range.
    using urng1_iterator_t = maybe_const_iterator_t<const_range, urng1_t>;
    //!\brief The iterator type of the second underlying range.
    using urng2_iterator_t = maybe_const_iterator_t<const_range, urng2_t>;

    template <bool>
    friend class basic_iterator;

public:
    /*!\name Associated types
     * \{
     */
    //!\brief Type for distances between iterators.
    using difference_type = std::ranges::range_difference_t<urng1_t>;
    //!\brief Value type of this iterator.
    using value_type = std::ranges::range_value_t<urng2_t>;
    //!\brief The pointer type.
    using pointer = void;
    //!\brief Reference to `value_type`.
    using reference = value_type;
    //!\brief Tag this class as a forward iterator.
    using iterator_category = std::forward_iterator_tag;
    //!\brief Tag this class as a forward iterator.
    using iterator_concept = iterator_category;
    //!\}

    /*!\name Constructors, destructor and assignment
     * \{
     */
    basic_iterator() = default; //!< Defaulted.
    basic_iterator(basic_iterator const &) = default; //!< Defaulted.
    basic_iterator(basic_iterator &&) = default; //!< Defaulted.
    basic_iterator & operator=(basic_iterator const &) = default; //!< Defaulted.
    basic_iterator & operator=(basic_iterator &&) = default; //!< Defaulted.
    ~basic_iterator() = default; //!< Defaulted.

    //!\brief Allow iterator on a const range to be constructible from an iterator over a non-const range.
    basic_iterator(basic_iterator<!const_range> const & it)
    //!\cond
        requires const_range
    //!\endcond
        : opensyncmer_value{std::move(it.opensyncmer_value)},
          urng1_iterator{std::move(it.urng1_iterator)},
          urng2_iterator{std::move(it.urng2_iterator)},
          urng1_sentinel{std::move(it.urng1_sentinel)}
    {}

    /*!\brief Construct from begin and end iterators of a given range over std::totally_ordered values, and the number
              of values per window.
    * \param[in] urng1_iterator Iterator pointing to the first position of the first std::totally_ordered range.
    * \param[in] urng2_iterator Iterator pointing to the first position of the first std::totally_ordered range.
    * \param[in] urng1_sentinel Iterator pointing to the last position of the first std::totally_ordered range.
    * \param[in] K The k-mer size used.
    * \param[in] S The s-mer size used.
    *
    * \details
    *
    * Looks at the number of values per window in two ranges, returns the smallest between both as opensyncmer and
    * shifts then by one to repeat this action. If a opensyncmer in consecutive windows is the same, it is returned only
    * once.
    */
    basic_iterator(urng1_iterator_t urng1_iterator,
                   urng2_iterator_t urng2_iterator,
                   urng1_sentinel_t urng1_sentinel,
                   size_t K,
                   size_t S) :
        urng1_iterator{std::move(urng1_iterator)},
        urng2_iterator{std::move(urng2_iterator)},
        urng1_sentinel{std::move(urng1_sentinel)}
    {
        window_first(K, S);
    }
    //!\}

    //!\anchor basic_iterator_comparison_opensyncmer
    //!\name Comparison operators
    //!\{

    //!\brief Compare to another basic_iterator.
    friend bool operator==(basic_iterator const & lhs, basic_iterator const & rhs)
    {
        return (lhs.urng1_iterator == rhs.urng1_iterator);
    }

    //!\brief Compare to another basic_iterator.
    friend bool operator!=(basic_iterator const & lhs, basic_iterator const & rhs)
    {
        return !(lhs == rhs);
    }

    //!\brief Compare to the sentinel of the opensyncmer_view.
    friend bool operator==(basic_iterator const & lhs, sentinel const &)
    {
        return lhs.urng1_iterator == lhs.urng1_sentinel;
    }

    //!\brief Compare to the sentinel of the opensyncmer_view.
    friend bool operator==(sentinel const & lhs, basic_iterator const & rhs)
    {
        return rhs == lhs;
    }

    //!\brief Compare to the sentinel of the opensyncmer_view.
    friend bool operator!=(sentinel const & lhs, basic_iterator const & rhs)
    {
        return !(lhs == rhs);
    }

    //!\brief Compare to the sentinel of the opensyncmer_view.
    friend bool operator!=(basic_iterator const & lhs, sentinel const & rhs)
    {
        return !(lhs == rhs);
    }
    //!\}

    //!\brief Pre-increment.
    basic_iterator & operator++() noexcept
    {
        next_unique_opensyncmer();
        return *this;
    }

    //!\brief Post-increment.
    basic_iterator operator++(int) noexcept
    {
        basic_iterator tmp{*this};
        next_unique_opensyncmer();
        return tmp;
    }

    //!\brief Return the opensyncmer.
    value_type operator*() const noexcept
    {
        return opensyncmer_value;
    }

private:
    //!\brief The opensyncmer value.
    value_type opensyncmer_value{};

    //!\brief The offset relative to the beginning of the window where the opensyncmer value is found.
    size_t opensyncmer_position_offset{};

    //!\brief Iterator to the rightmost value of one kmer.
    urng1_iterator_t urng1_iterator{};

    //!\brief Iterator to the rightmost value of one kmer in the second range.
    urng2_iterator_t urng2_iterator{};

    //!brief Iterator to last element in range.
    urng1_sentinel_t urng1_sentinel{};

    //!\brief The number of values in one window.
    size_t w_size{};

    //!\brief Stored values per window. It is necessary to store them, because a shift can remove the current opensyncmer.
    std::deque<value_type> window_values{};

    //!\brief Increments iterator by 1.
    void next_unique_opensyncmer()
    {
        while (!next_opensyncmer()) {}
    }

    //!\brief Returns new window value.
    auto window_value() const
    {
        return *urng1_iterator;
    }


    //!\brief Advances the window to the next position.
    void advance_window()
    {
        ++urng1_iterator;
        ++urng2_iterator;
    }

    //!\brief Advances the first window to the next position.
    void advance_first_window()
    {
        ++urng1_iterator;
    }


    //!\brief Calculates opensyncmers for the first window.
    void window_first(const size_t K, const size_t S)
    {
	w_size = K - S + 1;

	if (w_size == 0u)
            return;

        for (int i = 1u; i < K - 1 ; ++i)
        {
            window_values.push_back(window_value());
            advance_first_window();
        }
        window_values.push_back(window_value());


        auto smallest_s_it = std::ranges::min_element(window_values, std::less<value_type>{});
	    opensyncmer_position_offset = std::distance(std::begin(window_values), smallest_s_it);

	if (opensyncmer_position_offset == 0) {
		auto opensyncmer_it = urng2_iterator;
		opensyncmer_value = *opensyncmer_it;
	}

    }

    /*!\brief Calculates the next opensyncmer value.
     * \returns True, if new opensyncmer is found or end is reached. Otherwise returns false.
     * \details
     * For the following windows, we remove the first window value (is now not in window_values) and add the new
     * value that results from the window shifting.
     */
    bool next_opensyncmer()
    {
    	advance_window();


        if (urng1_iterator == urng1_sentinel)
            return true;

        value_type const new_value = window_value();

        window_values.pop_front();
        window_values.push_back(new_value);

	if (opensyncmer_position_offset == 0)
	{

		auto smallest_s_it = std::ranges::min_element(window_values, std::less<value_type>{});

		opensyncmer_position_offset = std::distance(std::begin(window_values), smallest_s_it);

		if (opensyncmer_position_offset == 0) {

			auto opensyncmer_it = urng2_iterator;
			opensyncmer_value = *opensyncmer_it;
			return true;
		};
	}

	else if (new_value < *(window_values.begin()+(opensyncmer_position_offset-1)))
	     {
	          opensyncmer_position_offset = w_size - 1;
	          return false;
	     }
	 else if (opensyncmer_position_offset == 1){
		  auto opensyncmer_it = urng2_iterator;
		  opensyncmer_value = *opensyncmer_it;
		  --opensyncmer_position_offset;
		  return true;
	 };



	--opensyncmer_position_offset;
	return false;
    }
};



//!\brief A deduction guide for the view class template.
template <std::ranges::viewable_range rng1_t, std::ranges::viewable_range rng2_t>
opensyncmer_view(rng1_t &&, rng2_t &&, size_t const K, size_t const S) -> opensyncmer_view<std::views::all_t<rng1_t>, std::views::all_t<rng2_t>>;



// ---------------------------------------------------------------------------------------------------------------------
// opensyncmer_fn (adaptor definition)
// ---------------------------------------------------------------------------------------------------------------------

//![adaptor_def]
//!\brief opensyncmer's range adaptor object type (non-closure).
//!\ingroup search_views
struct opensyncmer_fn
{
    //!\brief Store the number of values in one window and return a range adaptor closure object.
    template <std::ranges::range urng2_t>
    constexpr auto operator()(urng2_t urange2, const size_t K, const size_t S) const
    {
        return adaptor_from_functor{*this, urange2, K, S};
    }

    /*!\brief Call the view's constructor with two arguments: the underlying view and an integer indicating how many
     *        values one window contains.
     * \tparam urng1_t        The type of the input range to process. Must model std::ranges::viewable_range.
     * \tparam urng2_t        The type of the input range to process. Must model std::ranges::viewable_range.
     * \param[in] urange1     The input range to process. Must model std::ranges::viewable_range and
     *                        std::ranges::forward_range.
     * \param[in] urange2     The input range to process. Must model std::ranges::viewable_range and
     *                        std::ranges::forward_range.
     * \param[in] K The k-mer size used.
     * \param[in] S The s-mer size used.
     * \returns  A range of converted values.
     */
    template <std::ranges::range urng1_t, std::ranges::range urng2_t>
    constexpr auto operator()(urng1_t && urange1, urng2_t && urange2, size_t const K, size_t const S) const
    {
        static_assert(std::ranges::viewable_range<urng1_t>,
                      "The range parameter to views::opensyncmer cannot be a temporary of a non-view range.");
        static_assert(std::ranges::forward_range<urng1_t>,
                      "The range parameter to views::opensyncmer must model std::ranges::forward_range.");

        if (K < 1 || S < 0)  // Would just return urange1 without any changes
            throw std::invalid_argument{"The chosen K-mer or S-mer are not valid. "
                                        "Please choose a value that satisfize the given condition."};

        return opensyncmer_view{urange1, urange2, K, S};
    }
};
//![adaptor_def]

} // namespace seqan3::detail

namespace seqan3::views
{
/*!\brief Computes opensyncmers for a range of comparable values. A opensyncmer is a kmer that has the its smallest smer at its start.
 * \tparam urng_t The type of the first range being processed. See below for requirements. [template
 *                 parameter is omitted in pipe notation]
 * \param[in] urange1     The input range to process. Must model std::ranges::viewable_range and
 *                        std::ranges::forward_range.
 * \param[in] urange2     The input range to process. Must model std::ranges::viewable_range and
 *                        std::ranges::forward_range.
 * \param[in] K The k-mer size used.
 * \param[in] S The s-mer size used.
 * \returns A range of std::totally_ordered where each value is ... See below for the
 *          properties of the returned range.
 * \ingroup search_views
 *
 *
 * ### View properties
 *
 * | Concepts and traits              | `urng_t` (underlying range type)   | `rrng_t` (returned range type)   |
 * |----------------------------------|:----------------------------------:|:--------------------------------:|
 * | std::ranges::input_range         | *required*                         | *preserved*                      |
 * | std::ranges::forward_range       | *required*                         | *preserved*                      |
 * | std::ranges::bidirectional_range |                                    | *lost*                           |
 * | std::ranges::random_access_range |                                    | *lost*                           |
 * | std::ranges::contiguous_range    |                                    | *lost*                           |
 * |                                  |                                    |                                  |
 * | std::ranges::viewable_range      | *required*                         | *guaranteed*                     |
 * | std::ranges::view                |                                    | *guaranteed*                     |
 * | std::ranges::sized_range         |                                    | *lost*                           |
 * | std::ranges::common_range        |                                    | *lost*                           |
 * | std::ranges::output_range        |                                    | *lost*                           |
 * | seqan3::const_iterable_range     |                                    | *preserved*                      |
 * |                                  |                                    |                                  |
 * | std::ranges::range_reference_t   | std::totally_ordered               | std::totally_ordered             |
 *
 * See the views views submodule documentation for detailed descriptions of the view properties.
 */
inline constexpr auto opensyncmer = detail::opensyncmer_fn{};

} // namespace seqan3::views