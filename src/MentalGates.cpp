///////////////////////////////////////////////////
//
//   Gates VCV Module
//
//   Strum 2017
//
///////////////////////////////////////////////////

#include "mental.hpp"

//#include "dsp/digital.hpp"
/////////////////////////////////////////////////

struct MentalGates : Module {
	enum ParamIds {
      BUTTON_PARAM,
      NUM_PARAMS = BUTTON_PARAM + 4
	};

	enum InputIds {
      INPUT,
      GATE_INPUT = INPUT + 4,
      NUM_INPUTS = GATE_INPUT + 4
	};
	enum OutputIds {
      OUTPUT,
      NUM_OUTPUTS = OUTPUT + 4
	};
  enum LightIds {
		BUTTON_LIGHTS,
    ON_LEDS = BUTTON_LIGHTS + 4,
		NUM_LIGHTS = ON_LEDS + 4
	};

  SchmittTrigger button_triggers[4];
  bool button_on[4] = {0,0,0,0};
  float signal[4] = {0.0,0.0,0.0};
  float on[4] = {0.0,0.0,0.0};
  
	MentalGates() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);}
	void step() override;
  
  json_t *dataToJson() override
  {
		json_t *rootJ = json_object();
    
    // button states
		json_t *button_statesJ = json_array();
		for (int i = 0; i < 4; i++)
    {
			json_t *button_stateJ = json_integer((int) button_on[i]);
			json_array_append_new(button_statesJ, button_stateJ);
		}
		json_object_set_new(rootJ, "buttons", button_statesJ);    
    return rootJ;
  }
  
  void dataFromJson(json_t *rootJ) override
  {
    // button states
		json_t *button_statesJ = json_object_get(rootJ, "buttons");
		if (button_statesJ)
    {
			for (int i = 0; i < 4; i++)
      {
				json_t *button_stateJ = json_array_get(button_statesJ, i);
				if (button_stateJ)
					button_on[i] = !!json_integer_value(button_stateJ);
			}
		}  
  }
};

/////////////////////////////////////////////////////

void MentalGates::step() {

for (int i = 0 ; i < 4 ; i++)
  {
    signal[i] = inputs[INPUT + i].value;
    on[i] = inputs[GATE_INPUT + i].value;
  
    if (button_triggers[i].process(params[BUTTON_PARAM + i].value))
    {
	    button_on[i] = !button_on[i];
    }
    lights[BUTTON_LIGHTS + i ].value  = (button_on[i]) ? 1.0 : 0.0;

    if (button_on[i] || ( on[i] > 0.0))
    {
      outputs[OUTPUT + i].value = 0.0;
      lights[ON_LEDS + i].value = 1.0;
    }
    else
    {
      outputs[OUTPUT + i].value = signal[i];
      lights[ON_LEDS + i].value = 0.0;
    }
  }
}

//////////////////////////////////////////////////////////////////
struct MentalGatesWidget : ModuleWidget {
  MentalGatesWidget(MentalGates *module);
};

MentalGatesWidget::MentalGatesWidget(MentalGates *module) : ModuleWidget(module)
{

setPanel(SVG::load(assetPlugin(pluginInstance, "res/MentalGates.svg")));
  //TODO make this better somehow
  if(module == nullptr) {
    return;
  }
  int group_spacing = 85;
  for (int i = 0 ; i < 4 ; i++)
  {
	  addInput(createPort<InPort>(Vec(3, group_spacing * i +  60), PortWidget::INPUT, module, MentalGates::INPUT + i));
    addInput(createPort<GateInPort>(Vec(3, group_spacing * i +  28), PortWidget::INPUT, module, MentalGates::GATE_INPUT + i));
    addOutput(createPort<OutPort>(Vec(32, group_spacing * i +  60), PortWidget::OUTPUT, module, MentalGates::OUTPUT + i));

    addChild(createLight<MedLight<BlueLED>>(Vec(26, group_spacing * i + 17), module, MentalGates::ON_LEDS + i));
    addParam(createParam<LEDButton>(Vec(35, group_spacing * i +  31), module, MentalGates::BUTTON_PARAM + i, 0.0, 1.0, 0.0));
	  addChild(createLight<MedLight<BlueLED>>(Vec(35+5, group_spacing * i +  31+5), module, MentalGates::BUTTON_LIGHTS + i));  
  }
}

Model *modelMentalGates = createModel<MentalGates, MentalGatesWidget>("MentalGates");