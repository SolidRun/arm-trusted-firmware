/* Copyright 2017 NXP */
/* Include file detailing the resource partitioning for ATF */

/* resources that are going to stay in secure partition */
sc_rsrc_t secure_rsrcs[] = {
  SC_R_MU_1A,
  SC_R_A53,
  SC_R_A53_0,
  SC_R_A53_1,
  SC_R_A53_2,
  SC_R_A53_3,
  SC_R_A72,
  SC_R_A72_0,
  SC_R_A72_1,
  SC_R_GIC,
  SC_R_GIC_SMMU,
  SC_R_CCI
};

/* resources that have register access for non-secure domain */
sc_rsrc_t ns_access_allowed[] = {
  SC_R_GIC,
  SC_R_GIC_SMMU,
  SC_R_CCI
};
