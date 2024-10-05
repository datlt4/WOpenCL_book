__kernel void transfer_global_to_local(__global int4 *in_data,
                                       __global int4 *out_data) {
  __local int4 local_data;
  int id = get_local_id(0);
  local_data = in_data[id];
  out_data[id] = local_data;
}
