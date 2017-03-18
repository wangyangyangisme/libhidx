#include "Window.hh"

#include "TreeModel.hh"
#include "DeviceSelector.hh"
#include "hid/Collection.hh"

#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTreeView>
#include <QSettings>

#include <cassert>

namespace hidviz {

    Window::Window() : QWidget{} {
        auto layout = new QGridLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        setLayout(layout);

        auto devicesButton = new QPushButton{};
//    devicesButton->setFixedSize(100, 75);
        auto devicesButtonLayout = new QVBoxLayout{};
        auto usbLabel = new QLabel{};
        auto usbImage = QImage{"../../img/usb.png"};
        auto usbPixmap = QPixmap::fromImage(usbImage);
        usbPixmap = usbPixmap.scaledToWidth(64, Qt::SmoothTransformation);
        usbLabel->setPixmap(usbPixmap);
        usbLabel->setFixedSize(usbPixmap.size());
        devicesButtonLayout->addWidget(usbLabel, 0, Qt::AlignCenter);
        devicesButtonLayout->addWidget(new QLabel{"Devices"}, 0,
                                       Qt::AlignCenter);
        devicesButtonLayout->setSizeConstraint(QLayout::SetMinimumSize);

        devicesButton->setLayout(devicesButtonLayout);
        QObject::connect(devicesButton, &QPushButton::pressed, this,
                         &Window::openDeviceSelector);
//    devicesButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        layout->addWidget(devicesButton, 0, 0);

        auto vSeparator1 = new QFrame{};
        vSeparator1->setLineWidth(1);
        vSeparator1->setFrameShape(QFrame::VLine);
        vSeparator1->setPalette(QPalette{QColor{0, 0, 0}});
        vSeparator1->setMaximumWidth(1);
        layout->addWidget(vSeparator1, 0, 1);

        deviceName = new QLabel{"Logitech, Inc. Unifying Receiver"};
        QFont f;
        f.setPointSize(32);
        deviceName->setFont(f);
        deviceName->setContentsMargins(20, 0, 0, 0);
        layout->addWidget(deviceName, 0, 2);

        auto separator = new QFrame{};
        separator->setLineWidth(1);
        separator->setMidLineWidth(0);
        separator->setContentsMargins(0, 0, 0, 0);
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Raised);
        separator->setPalette(QPalette{QColor{0, 0, 0}});
        separator->setMaximumHeight(1);
        layout->addWidget(separator, 1, 0, 1, 3);

        auto menuLayout = new QVBoxLayout{};
        auto a = new QPushButton{"Descriptors"};
        menuLayout->insertStretch(0, 0);
        menuLayout->addWidget(a, 0, Qt::AlignBottom);
        menuLayout->addWidget(new QPushButton{"Reports"}, 0, Qt::AlignBottom);


        layout->addLayout(menuLayout, 2, 0);


        auto vSeparator2 = new QFrame{};
        vSeparator2->setLineWidth(1);
        vSeparator2->setFrameShape(QFrame::VLine);
        vSeparator2->setPalette(QPalette{QColor{0, 0, 0}});
        vSeparator2->setMaximumWidth(1);
        layout->addWidget(vSeparator2, 2, 1);

        content = new QTreeView;
        content->setSizePolicy(
            QSizePolicy{QSizePolicy::Expanding, QSizePolicy::Expanding});
        content->setSelectionMode(QTreeView::NoSelection);
        layout->addWidget(content, 2, 2);
        QSettings settings{"hidviz"};
        restoreGeometry(settings.value( "geometry", saveGeometry() ).toByteArray());
        move(settings.value( "pos", pos() ).toPoint());
        resize(settings.value( "size", size() ).toSize());

        show();
    }

    void Window::openDeviceSelector() {
        auto dialog = new DeviceSelector;
        dialog->show();
        connect(dialog, &DeviceSelector::deviceSelected, this,
                &Window::selectDevice);
    }

    void Window::selectDevice(const libhidx::Interface& interface) {
        assert(interface.isHid());

        deviceName->setText(QString::fromStdString(interface.getName()));

        auto model = new TreeModel{interface.getRootCollection()};
        content->setModel(model);
        model->forEach([this](const QModelIndex& index){
            if(index.column() == 0){
                return;
            }
            auto item = static_cast<libhidx::hid::Item*>(index.data(Qt::UserRole).value<void*>());
            content->setIndexWidget(index, new hid::Collection{item});
        });
    }

    void Window::closeEvent(QCloseEvent* event) {
        QSettings settings{"hidviz"};
        settings.setValue( "geometry", saveGeometry() );
        settings.setValue( "maximized", isMaximized() );
        if ( !isMaximized() ) {
            settings.setValue( "pos", pos() );
            settings.setValue( "size", size() );
        }
        QWidget::closeEvent(event);
    }

}