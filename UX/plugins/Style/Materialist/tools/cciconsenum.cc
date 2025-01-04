#include <cc/File>
#include <cc/json>
#include <cc/stdio>
#include <cc/chars>
#include <cc/debugging>

namespace cc {

String camelize(const String &name)
{
    List<String> parts = name.split('-');
    for (String &part: parts) {
        if (part.count() > 0) part[0] = toUpper(part[0]);
    }
    return parts.join();
}

} // namespace cc

int main()
{
    using namespace cc;

    try {

        const String path = String{__FILE__}.cdUp().cdUp() / "src" / "icons" / "meta.json";

        Variant value = jsonParse(File{path}.map());
        List<Variant> items = value.to<List<Variant>>();

        uint64_t codepointFirst = ~UINT64_C(0);
        uint64_t codepointLast  = 0;

        Map<String, uint64_t> codepointByName;

        for (const Variant &item: items) {
            uint64_t codepoint = 0;
            MetaObject record = item.to<MetaObject>();
            if (!record("codepoint").to<String>().readNumber<uint64_t, 16>(&codepoint)) throw String{"Failed to read codepoint"};
            String name = record("name").to<String>();
            if (!codepointByName.insert(camelize(name), codepoint)) throw (String{"Failed to redefine codepoint "} + name).join();
            /*if (record.contains("aliases")) {
                List<Variant> aliases = item("aliases").to<List<Variant>>();
                for (const Variant &alias: aliases) {
                    codepointByName.insert(camelize(alias), codepoint);
                }
            }*/
            if (codepoint < codepointFirst) codepointFirst = codepoint;
            if (codepoint > codepointLast) codepointLast = codepoint;
        }

        #if 0
        long lenMax = 0;

        for (const auto &pair: codepointByName) {
            const String &name = pair.key();
            if (name.count() > lenMax) lenMax = name.count();
        }

        for (const auto &pair: codepointByName) {
            const String &name = pair.key();
            const uint64_t codepoint = pair.value();
            const String padding = String::allocate(lenMax - name.count(), ' ');
            fout() << name << padding << " = 0x" << hex(codepoint) << "," << nl;
        }

        fout()
            << "First" << String::allocate(lenMax - 5, ' ') << " = 0x" << hex(codepointFirst) << "," << nl
            << "Last" << String::allocate(lenMax - 4, ' ') << " = 0x" << hex(codepointLast) << nl
            << nl;
        #else
        for (const auto &pair: codepointByName) {
            const String &name = pair.key();
            fout("{ Icon::%%, \"%%\" },\n") << name << name;
        }
        #endif
    }
    catch (String &error)
    {
        ferr() << error << nl;
    }

    return 0;
}
