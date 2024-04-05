/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/TypeInfoSource>
#include <cc/isobus/pd/TypeInfoSyntax>
#include <cc/DEBUG>

namespace cc::isobus::pd {

struct TypeInfoSource::State final: public Object::State
{
    explicit State(const String &text):
        text_{text},
        offset_{0}
    {
        if (text.find("\n\n\n", &offset_)) {
            offset_ += 3;
        }
    }

    bool read(Out<TypeInfo> info)
    {
        if (offset_ == text_.count()) {
            return false;
        }
        Token token = syntax_.match(text_, offset_);
        if (!token) {
            throw TextError{text_, offset_, "Broken record"};
        }
        offset_ = token.i1();
        read(token, info);
        return token;
    }

    void read(const Token &root, Out<TypeInfo> info)
    {
        info = TypeInfo{text_.select(root), root.i0()};

        for (const Token &token: root.children())
        {
            assert(token.children().count() == 2);
            const Token &keyToken = token.children().first();
            const Token &valueToken = token.children().last();
            String key = text_.copy(keyToken);
            String value = text_.copy(valueToken);

            if (key == "DD Entity") {
                long i0 = valueToken.i0();
                long i1 = i0;
                if (text_.find(' ', &i1)) {
                    uint32_t typeId = 0;
                    if (!scanNumber<uint32_t>(text_, &typeId, 10, i0, i1)) {
                        throw TextError{text_, i0, "Expected type ID (DDI)"};
                    }
                    info->setTypeId(typeId);
                }
                else {
                    throw TextError{text_, i0, "Expected type ID (DDI)"};
                }

                ++i1;
                i0 = i1;
                if (text_.find('\n', &i1)) {
                    String typeName = text_.copy(i0, i1);
                    long j = 0;
                    if (typeName.find(" as [", &j)) typeName.truncate(j);
                    info->setTypeName(typeName);
                }
                else {
                    throw TextError{text_, i0, "Expected type name"};
                }
            }
            else if (key == "Definition") {
                info->setDefinition(value);
            }
            else if (key == "CANBus Range") {
                if (value == "- ") continue;
                List<String> parts = value.split(" - ");
                if (parts.count() != 2) {
                    throw TextError{text_, valueToken.i0(), "Expected range"};
                }
                int ok = 0;
                int64_t min = readNumber<int64_t>(parts.at(0), 0, &ok);
                int64_t max = readNumber<int64_t>(parts.at(1), 0, &ok);
                if (ok != 2) {
                    throw TextError{text_, valueToken.i0(), "Expected range"};
                }
                info->setBusValueMin(min);
                info->setBusValueMax(max);
            }
            else if (key == "Display Range") {
                if (value == "- ") continue;
                List<String> parts = value.split(" - ");
                if (parts.count() != 2) {
                    throw TextError{text_, valueToken.i0(), "Expected range"};
                }
                parts[0].replace(',', '.');
                parts[1].replace(',', '.');
                int ok = 0;
                double min = readNumber<double>(parts.at(0), 0, &ok);
                double max = readNumber<double>(parts.at(1), 0, &ok);
                if (ok != 2) {
                    throw TextError{text_, valueToken.i0(), "Expected range"};
                }
                info->setScaledValueMin(min);
                info->setScaledValueMax(max);
            }
            else if (key == "Resolution") {
                int ok = 0;
                value.replace(',', '.');
                double scale = readNumber<double>(value, &ok);
                if (ok != 1) {
                    throw TextError{text_, valueToken.i0(), "Expected number"};
                }
                info->setScale(scale);
            }
            else if (key == "Unit") {
                if (value.startsWith("n.a.") || value == "not defined" || value == "") continue;
                if (value.contains('(')) {
                    value = value.replaced("(", "- ");
                    value.replace(")", "");
                }
                List<String> parts = value.split(" - ");
                if (parts.count() != 2) {
                    throw TextError{text_, valueToken.i0(), "Expected unit"};
                }
                if (parts.at(0) == "not defined") continue;
                info->setUnit(parts.at(0));
                info->setUnitName(parts.at(1));
            }
        }
    }

    TypeInfoSyntax syntax_;
    String text_;
    long offset_;
};

TypeInfoSource::TypeInfoSource(const String &text):
    Object{new State{text}}
{}

bool TypeInfoSource::read(Out<TypeInfo> info)
{
    return me().read(info);
}

TypeInfoSource::State &TypeInfoSource::me()
{
    return Object::me.as<State>();
}

} // namespace cc::isobus::pd
