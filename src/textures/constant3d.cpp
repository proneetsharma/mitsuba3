#include <mitsuba/core/properties.h>
#include <mitsuba/core/transform.h>
#include <mitsuba/render/texture.h>

NAMESPACE_BEGIN(mitsuba)

template <typename Float, typename Spectrum>
class Constant3D final : public Texture3D<Float, Spectrum> {
public:
    MTS_IMPORT_BASE(Texture3D, is_inside, m_world_to_local)
    MTS_IMPORT_TYPES(Texture)

    explicit Constant3D(const Properties &props) : Base(props) {
        m_color = props.texture<Texture>("color", 1.f);
    }

    UnpolarizedSpectrum eval(const Interaction3f &it, Mask active) const override {
        return eval_impl<false>(it, active);
    }

    Float eval_1(const Interaction3f & /* it */, Mask /* active */) const override {
        return m_color->mean();
    }

    std::pair<UnpolarizedSpectrum, Vector3f> eval_gradient(const Interaction3f &it,
                                                Mask active) const override {
        return eval_impl<true>(it, active);
    }

    template <bool with_gradient>
    MTS_INLINE auto eval_impl(const Interaction3f &it, const Mask &active) const {
        Mask inside    = active && is_inside(it, active);

        SurfaceInteraction3f si;
        si.uv          = Point2f(0.f, 0.f);
        si.wavelengths = it.wavelengths;
        si.time        = it.time;
        auto result = m_color->eval(si, active);

        if constexpr (with_gradient)
            return std::make_pair(result, zero<Vector3f>());
        else
            return result;
    }

    Mask is_inside(const Interaction3f &it, Mask /*active*/) const override {
        auto p = m_world_to_local * it.p;
        return all((p >= 0) && (p <= 1));
    }

    ScalarFloat max() const override { NotImplementedError("max"); }

    void traverse(TraversalCallback *callback) override {
        callback->put_object("color", m_color.get());
    }

    std::string to_string() const override {
        std::ostringstream oss;
        oss << "Constant3D[" << std::endl
            << "  world_to_local = " << m_world_to_local << "," << std::endl
            << "  color = " << m_color << std::endl
            << "]";
        return oss.str();
    }

    MTS_DECLARE_CLASS()
protected:
    ref<Texture> m_color;
};

MTS_IMPLEMENT_CLASS_VARIANT(Constant3D, Texture3D)
MTS_EXPORT_PLUGIN(Constant3D, "Constant 3D texture")
NAMESPACE_END(mitsuba)
