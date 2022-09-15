#include <../include/rank_op.hpp>

inline void spb::Rank::rank_op(spb::item_data &item){
	cass_query_t query;
	query = item.first.rank.query;

	cass_result_t *candidate;

	item.first.rank.name = item.second.vec.name;

	query.flags = CASS_RESULT_LIST | CASS_RESULT_USERMEM | CASS_RESULT_SORT;
	query.dataset = item.second.vec.ds;
	query.vecset_id = 0;

	query.vec_dist_id = IO_data_vec[item.sourceId].inputData.vec_dist_id;

	query.vecset_dist_id = IO_data_vec[item.sourceId].inputData.vecset_dist_id;

	query.topk = IO_data_vec[item.sourceId].inputData.top_K;

	query.extra_params = NULL;

	candidate = cass_result_merge_lists(&item.second.vec.result,
			(cass_dataset_t *)IO_data_vec[item.sourceId].inputData.query_table->__private,
			0);
	query.candidate = candidate;

	cass_result_alloc_list(&item.first.rank.result,
			0, IO_data_vec[item.sourceId].inputData.top_K);
	cass_table_query(IO_data_vec[item.sourceId].inputData.query_table, &query,
			&item.first.rank.result);

	cass_result_free(&item.second.vec.result);
	cass_result_free(candidate);
	free(candidate);
	cass_dataset_release(item.second.vec.ds);
}