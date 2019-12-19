#include <mitsuba/render/texture.h>
#include <mitsuba/render/srgb.h>
#include <mitsuba/core/plugin.h>
#include <mitsuba/core/properties.h>

NAMESPACE_BEGIN(mitsuba)

template <typename Float, typename Spectrum>
class SRGBEmitterSpectrum final : public Texture<Float, Spectrum> {
public:
    MTS_IMPORT_TYPES(Texture)

    SRGBEmitterSpectrum(const Properties &props) : Texture(props) {
        ScalarColor3f color = props.color("color");

        if constexpr (is_spectral_v<Spectrum>) {
            /* Evaluate the spectral upsampling model. This requires a
               reflectance value (colors in [0, 1]) which is accomplished here by
               scaling. We use a color where the highest component is 50%,
               which generally yields a fairly smooth spectrum. */
            ScalarFloat scale = hmax(color) * 2.f;
            if (scale != 0.f)
                color /= scale;

            m_value = srgb_model_fetch(color);

            Properties props2("d65");
            props2.set_float("scale", props.float_("scale", 1.f) * scale);
            PluginManager *pmgr = PluginManager::instance();
            m_d65 = (Texture *) pmgr->create_object<Texture>(props2)->expand().at(0).get();
        } else if constexpr (is_rgb_v<Spectrum>) {
            m_value = color;
        } else {
            static_assert(is_monochromatic_v<Spectrum>);
            m_value = luminance(color);
        }
    }

    UnpolarizedSpectrum eval(const SurfaceInteraction3f &si, Mask active) const override {
        if constexpr (is_spectral_v<Spectrum>)
            return m_d65->eval(si, active) *
                   srgb_model_eval<UnpolarizedSpectrum>(m_value, si.wavelengths);
        else
            return m_value;
    }

    void traverse(TraversalCallback *callback) override {
        callback->put_parameter("value", m_value);
    }

    MTS_DECLARE_CLASS()
private:
    /**
     * Depending on the compiled variant, this plugin either stores coefficients
     * for a spectral upsampling model, or a plain RGB/monochromatic value.
     */
    static constexpr size_t ChannelCount = is_monochromatic_v<Spectrum> ? 1 : 3;

    Array<ScalarFloat, ChannelCount> m_value;
    ref<Texture> m_d65;
};

MTS_IMPLEMENT_CLASS_VARIANT(SRGBEmitterSpectrum, Texture)
MTS_EXPORT_PLUGIN(SRGBEmitterSpectrum, "sRGB x D65 spectrum")
NAMESPACE_END(mitsuba)
