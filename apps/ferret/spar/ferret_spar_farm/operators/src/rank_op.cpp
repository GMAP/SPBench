#include <../include/rank_op.hpp>

using namespace spb;

inline void Rank::rank_op(item_data &item){
	cass_query_t query;
	query = item.first.rank.query;// item.query_batch[num_item];

	cass_result_t *candidate;

	item.first.rank.name = item.second.vec.name;

	query.flags = CASS_RESULT_LIST | CASS_RESULT_USERMEM | CASS_RESULT_SORT;
	query.dataset = item.second.vec.ds;
	query.vecset_id = 0;

	query.vec_dist_id = vec_dist_id;

	query.vecset_dist_id = vecset_dist_id;

	query.topk = top_K;

	query.extra_params = NULL;

	candidate = cass_result_merge_lists(&item.second.vec.result,
			(cass_dataset_t *)query_table->__private,
			0);
	query.candidate = candidate;

	cass_result_alloc_list(&item.first.rank.result,
			0, top_K);
	cass_table_query(query_table, &query,
			&item.first.rank.result);

	cass_result_free(&item.second.vec.result);
	cass_result_free(candidate);
	free(candidate);
	cass_dataset_release(item.second.vec.ds);
}