#include <seqan3/alphabet/nucleotide/dna4.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/search/views/kmer_hash.hpp>
#include <seqan3/search/views/syncmer.hpp>
#include <seqan3/search/views/opensyncmer_hash.hpp>
#include <seqan3/search/views/syncmer_hash.hpp>
#include <seqan3/search/views/minimiser_hash.hpp>
using namespace seqan3::literals;

int main()
{
    seqan3::seed seed = seqan3::seed{0};
    std::vector<seqan3::dna4> text{"ACGGCGACGTTTAG"_dna4};
    auto text_reversed = text
	                                 | seqan3::views::complement
	                                 | std::views::reverse;

    auto start_at_a = std::views::drop(6);
    auto stop_at_t = std::views::take_while([] (seqan3::dna4 const x) { return x != 'T'_dna4; });
    auto kmers = 5;
    auto smers = 2;

    auto kmers_forward_strand = text
	                                 | seqan3::views::kmer_hash(seqan3::shape(seqan3::ungapped(kmers)))
	                                 | std::views::transform([seed] (uint64_t i)
	                                          {return i ^ seed.get();});

    auto kmers_reverse_strand = text
	                                 | seqan3::views::complement
	                                 | std::views::reverse
	                                 | seqan3::views::kmer_hash(seqan3::shape(seqan3::ungapped(kmers)))
	                                 | std::views::transform([seed] (uint64_t i)
	                                          {return i ^ seed.get();})
	                                 | std::views::reverse;

    auto smers_forward_strand = text
	                                 | seqan3::views::kmer_hash(seqan3::shape(seqan3::ungapped(smers)))
	                                 | std::views::transform([seed] (uint64_t i)
	                                          {return i ^ seed.get();});

    auto smers_reverse_strand = text
	                                 | seqan3::views::complement
	                                 | std::views::reverse
	                                 | seqan3::views::kmer_hash(seqan3::shape(seqan3::ungapped(smers)))
	                                 | std::views::transform([seed] (uint64_t i)
	                                          {return i ^ seed.get();})
	                                 | std::views::reverse;


    auto kmers_combined_strand = seqan3::views::zip(kmers_forward_strand, kmers_reverse_strand)
	                                 | std::views::transform([seed](std::tuple<uint64_t, uint64_t> i)
	                                          {return std::min<size_t>(std::get<0>(i), std::get<1>(i));});

    auto smers_combined_strand = seqan3::views::zip(smers_forward_strand, smers_reverse_strand)
	                                 | std::views::transform([seed](std::tuple<uint64_t, uint64_t> i)
	                                          {return std::min<size_t>(std::get<0>(i), std::get<1>(i));});

    seqan3::debug_stream << text << "\n";

   seqan3::debug_stream << "kmers_forward_strand: " << kmers_forward_strand  << '\n';
   seqan3::debug_stream << "kmers_reverse_strand: " << kmers_reverse_strand  << '\n';
   seqan3::debug_stream << "kmers_combined_strand: " << kmers_combined_strand << '\n';
   seqan3::debug_stream << "smers_forward_strand: " << smers_forward_strand  << '\n';
   seqan3::debug_stream << "smers_reverse_strand: " << smers_reverse_strand  << '\n';
   seqan3::debug_stream << "smers_combined_strand: " << smers_combined_strand  << '\n';

   auto opensyncmer_forward = text | opensyncmer_hash(2, 5, seqan3::seed{0});
   auto syncmer_forward = text | syncmer_hash(2, 5, seqan3::seed{0});

   auto opensyncmer_reverse = text_reversed | opensyncmer_hash(2, 5, seqan3::seed{0});
   auto syncmer_reverse = text_reversed | syncmer_hash(2, 5, seqan3::seed{0});

   auto opensyncmer_stop = text | stop_at_t | opensyncmer_hash(2, 5, seqan3::seed{0});
   auto opensyncmer_sart = text | start_at_a | opensyncmer_hash(2, 5, seqan3::seed{0});
   auto syncmer_stop = text | stop_at_t | syncmer_hash(2, 5, seqan3::seed{0});
   auto syncmer_sart = text | start_at_a | syncmer_hash(2, 5, seqan3::seed{0});

   seqan3::debug_stream << "opensyncmer_sart: " << opensyncmer_sart << '\n';
   seqan3::debug_stream << "opensyncmer_stop: " << opensyncmer_stop << '\n';
   seqan3::debug_stream << "syncmer_sart: " << syncmer_sart << '\n';
   seqan3::debug_stream << "syncmer_stop: " << syncmer_stop << '\n';

   seqan3::debug_stream << "opensyncmer_forward: " << opensyncmer_forward << '\n';
   seqan3::debug_stream << "syncmer_forward: " << syncmer_forward << '\n';
   
   seqan3::debug_stream << "opensyncmer_reverse: " << opensyncmer_reverse << '\n';
   seqan3::debug_stream << "syncmer_reverse: " << syncmer_reverse << '\n';


};
