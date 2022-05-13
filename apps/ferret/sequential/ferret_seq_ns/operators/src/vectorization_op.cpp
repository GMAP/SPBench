#include <../include/vectorization_op.hpp>

inline void spb::Vectorization::vectorization_op(spb::item_data &item){

	cass_query_t query;
	query = item.second.vec.query;

	item.second.vec.name = item.extract.name;

	memset(&query, 0, sizeof query);
	query.flags = CASS_RESULT_LISTS | CASS_RESULT_USERMEM;

	item.second.vec.ds = query.dataset = &item.extract.ds;
	query.vecset_id = 0;

	query.vec_dist_id = IO_data_vec[item.sourceId].inputData.vec_dist_id;

	query.vecset_dist_id = IO_data_vec[item.sourceId].inputData.vecset_dist_id;

	query.topk = 2*IO_data_vec[item.sourceId].inputData.top_K;

	query.extra_params = extra_params;

	cass_result_alloc_list(&item.second.vec.result,
			item.second.vec.ds->vecset[0].num_regions,
			query.topk);

	cass_table_query(IO_data_vec[item.sourceId].inputData.table, &query, &item.second.vec.result);
	
	item.second.vec.query = query;
}