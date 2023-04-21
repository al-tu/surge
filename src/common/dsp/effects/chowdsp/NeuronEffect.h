/*
 * Surge XT - a free and open source hybrid synthesizer,
 * built by Surge Synth Team
 *
 * Learn more at https://surge-synthesizer.github.io/
 *
 * Copyright 2018-2023, various authors, as described in the GitHub
 * transaction log.
 *
 * Surge XT is released under the GNU General Public Licence v3
 * or later (GPL-3.0-or-later). The license is found in the "LICENSE"
 * file in the root of this repository, or at
 * https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 * Surge was a commercial product from 2004-2018, copyright and ownership
 * held by Claes Johanson at Vember Audio during that period.
 * Claes made Surge open source in September 2018.
 *
 * All source for Surge XT is available at
 * https://github.com/surge-synthesizer/surge
 */

#ifndef SURGE_SRC_COMMON_DSP_EFFECTS_CHOWDSP_NEURONEFFECT_H
#define SURGE_SRC_COMMON_DSP_EFFECTS_CHOWDSP_NEURONEFFECT_H

#include "Effect.h"
#include "BiquadFilter.h"
#include "ModControl.h"

#include <vembertech/lipol.h>

#include "shared/chowdsp_DelayLine.h"
#include "shared/Oversampling.h"
#include "shared/SmoothedValue.h"

namespace chowdsp
{

/*
** CHOWDSP Neuron:
** Neuron is a distortion effect based on  the Gated Recurrent Unit,
** a sort of "neuron" model commonly used in recurrent neural networks.
**
** For more details on the original idea and implementation of this effect,
** please see this Medium article:
*https://jatinchowdhury18.medium.com/complex-nonlinearities-episode-10-gated-recurrent-distortion-6d60948323cf
*/
class NeuronEffect : public Effect
{
  public:
    enum neuron_params
    {
        neuron_drive_wh = 0,
        neuron_squash_wf,
        neuron_stab_uf,
        neuron_asym_uh,
        neuron_bias_bf,

        neuron_comb_freq,
        neuron_comb_sep,

        neuron_lfo_wave,
        neuron_lfo_rate,
        neuron_lfo_depth,

        neuron_width,
        neuron_gain,

        neuron_num_params,
    };

    NeuronEffect(SurgeStorage *storage, FxStorage *fxdata, pdata *pd);
    virtual ~NeuronEffect();

    virtual const char *get_effectname() override { return "Neuron"; }

    virtual void init() override;
    virtual void process(float *dataL, float *dataR) override;
    virtual void suspend() override;

    virtual void init_ctrltypes() override;
    virtual void init_default_values() override;
    virtual const char *group_label(int id) override;
    virtual int group_label_ypos(int id) override;

  private:
    void set_params();
    void process_internal(float *dataL, float *dataR, const int numSamples);

    inline float processSample(float x, float yPrev) noexcept
    {
        float f = sigmoid(Wf.getNextValue() * x + Uf.getNextValue() * yPrev + bf.getNextValue());
        return f * yPrev +
               (1.0f - f) * std::tanh(Wh.getNextValue() * x + Uh.getNextValue() * f * yPrev);
    }

    inline float sigmoid(float x) const noexcept { return 1.0f / (1.0f + std::exp(-x)); }

    enum
    {
        numSteps = 200,
    };

    SmoothedValue<float, ValueSmoothingTypes::Linear> Wf = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Wh = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Uf = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> Uh = 0.5f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> bf = 0.0f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> delay1Smooth = 0.0f;
    SmoothedValue<float, ValueSmoothingTypes::Linear> delay2Smooth = 0.0f;

    float y1[2] = {0.0f, 0.0f};

    BiquadFilter dc_blocker;
    lipol_ps makeup alignas(16), width alignas(16), outgain alignas(16);
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Linear> delay1{1 << 18};
    chowdsp::DelayLine<float, chowdsp::DelayLineInterpolationTypes::Linear> delay2{1 << 18};
    Oversampling<2, BLOCK_SIZE> os;

    Surge::ModControl modLFO;
};

} // namespace chowdsp

#endif // SURGE_SRC_COMMON_DSP_EFFECTS_CHOWDSP_NEURONEFFECT_H
