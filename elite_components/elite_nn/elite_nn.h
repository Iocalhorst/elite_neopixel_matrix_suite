#pragma once


typedef struct{
  int num_input_neurons;
  int num_hidden_layers;
  int num_neurons_per_hidden_layer;
  int num_output_neurons;
}elite_nn_arch_t;


//snake has 3 degrees_of_freedom
//turn left, turn right, or don't turn at all
//lets put 2 hidden layers. cause thats what everybody else does.
//the food smells good. lets do wave propagation.

//so we have the option of doing wave propagation from the food back "out of there" ... which means "is there a path thats >= snake_len"
//3 times. from each of the 3 fields.

//
//whats in the field upfront and its neighbors (if the snakes head isnt among them then theres something fucked up)
//lets uses wave propagartion to come up with a distance to food.
//and run it from the fields left, right and upfront.
//thats 3+3


//checking each direction yields a distance to some "thing".
//that "thing"  is either food, or its not.
//which adds up to 5 input neurons.

//on each new "food" we
//generate a map of "food smell intensity" by wave propagation

//the values left,right and upfront represent 3 input activations
//and we pass the "terrain" next to the head. 3 more inputs.
//and its own lenth. maybe it does something with it. dunno




typedef struct {
  elite_nn_arch_t arch;
  void* p_data_blob;
}elite_nn_t;


elite_nn_t elite_nn_construct(elite_nn_arch_t arch);
