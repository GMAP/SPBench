#include <../include/vectorization_op.hpp>

using namespace spb;

inline void Vectorization::vectorization_op(item_data &item){

	cass_query_t query;
	query = item.second.vec.query;// item.query_batch[num_item];

	item.second.vec.name = item.extract.name;

	memset(&query, 0, sizeof query);
	query.flags = CASS_RESULT_LISTS | CASS_RESULT_USERMEM;

	item.second.vec.ds = query.dataset = &item.extract.ds;
	query.vecset_id = 0;

	query.vec_dist_id = vec_dist_id;

	query.vecset_dist_id = vecset_dist_id;

	query.topk = 2*top_K;

	query.extra_params = extra_params;

	cass_result_alloc_list(&item.second.vec.result,
			item.second.vec.ds->vecset[0].num_regions,
			query.topk);

	cass_table_query(table, &query, &item.second.vec.result);
	
	item.second.vec.query = query;
}