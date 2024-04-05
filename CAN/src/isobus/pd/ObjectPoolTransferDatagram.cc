/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/isobus/pd/ObjectPoolTransferDatagram>
#include <cc/isobus/pd/DeviceObject>
#include <cc/isobus/pd/DeviceElement>
#include <cc/isobus/pd/DeviceProcessData>
#include <cc/isobus/pd/DeviceProperty>
#include <cc/isobus/pd/ValuePresentation>
#include <cc/Format>

namespace cc::isobus::pd {

void ObjectPoolTransferDatagram::printValue(Format &f) const
{
    String objectData = String{data()}.select(1, data().count());
    f << "  \"device-objects\": [";

    for (bool firstTime = true; objectData.count() > 0;)
    {
        if (firstTime) {
            f << "\n    ";
            firstTime = false;
        }
        else {
            f << ",\n    ";
        }

        String objectText;
        long objectSize = 0;

        if (DeviceObject{objectData}) {
            DeviceObject object{objectData};
            objectText = object.toString();
            objectSize = object.byteSize();
        }
        else if (DeviceElement{objectData}) {
            DeviceElement object{objectData};
            objectText = object.toString();
            objectSize = object.byteSize();
        }
        else if (DeviceProcessData{objectData}) {
            DeviceProcessData object{objectData};
            objectText = object.toString();
            objectSize = object.byteSize();
        }
        else if (DeviceProperty{objectData}) {
            DeviceProperty object{objectData};
            objectText = object.toString();
            objectSize = object.byteSize();
        }
        else if (ValuePresentation{objectData}) {
            ValuePresentation object{objectData};
            objectText = object.toString();
            objectSize = object.byteSize();
        }
        else {
            f <<
                "{\n"
                "      \"type\": \"isobus.device.object_unknown\"\n"
                "      \"extra-bytes\": " << objectData.count() << "\n"
                "    }";
            break;
        }

        f << objectText.replaced("\n", "\n    ");
        objectData = objectData.select(objectSize, objectData.count());
    }

    f << "\n  ],\n";
}

} // namespace cc::isobus::pd
