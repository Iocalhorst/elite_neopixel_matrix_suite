#pragma once


typedef struct{
  int num_input_neurons;
  int num_hidden_layers;
  int num_neurons_per_hidden_layer;
  int num_output_neurons;
}elite_nn_arch_t;


//snake has 4 degrees_of_freedom
//checking each direction yields a distance to some "thing".
//that "thing"  is either food, or its not.
//which adds up to 5 input neurons.

smart_snake_arch={num_directions+1,1,4,num_directions};

typedef struct {
  elite_nn_arch_t arch;
  void* p_data_blob;
}elite_nn_t;


elite_nn_t elite_nn_construct(elite_nn_arch_t arch);
