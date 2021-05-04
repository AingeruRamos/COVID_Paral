int calcular_edad(int n,int alfa, int beta){
  int i, semilla, edad;
  float mu;
  gsl_rng *r;

  mu=100;
  semilla=1;

  gsl_rng_env_setup();
  r = gsl_rng_alloc(gsl_rng_default);
  gsl_rng_set(r, seed);

  edad = round(mu * gsl_ran_beta(r, alfa, beta));

  gsl_rng_free(r);
  return edad;
}
