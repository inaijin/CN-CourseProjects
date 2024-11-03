/****************************************************************************
** Meta object code from reading C++ file 'webrtc.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../webrtc.h"
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'webrtc.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSWebRTCENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSWebRTCENDCLASS = QtMocHelpers::stringData(
    "WebRTC",
    "openedDataChannel",
    "",
    "peerId",
    "closedDataChannel",
    "incomingTrack",
    "trackData",
    "localDescriptionGenerated",
    "peerID",
    "sdp",
    "localCandidateGenerated",
    "candidate",
    "sdpMid",
    "isOffererChanged",
    "gatheringCompleted",
    "offerIsReady",
    "description",
    "answerIsReady",
    "ssrcChanged",
    "payloadTypeChanged",
    "bitRateChanged",
    "peerConnected",
    "trackOpened",
    "setRemoteDescription",
    "setRemoteCandidate",
    "init",
    "id",
    "isOfferer",
    "addPeer",
    "generateOfferSDP",
    "generateAnswerSDP",
    "addTrack",
    "ssrc",
    "rtc::SSRC",
    "payloadType",
    "bitRate"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSWebRTCENDCLASS_t {
    uint offsetsAndSizes[72];
    char stringdata0[7];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[18];
    char stringdata5[14];
    char stringdata6[10];
    char stringdata7[26];
    char stringdata8[7];
    char stringdata9[4];
    char stringdata10[24];
    char stringdata11[10];
    char stringdata12[7];
    char stringdata13[17];
    char stringdata14[19];
    char stringdata15[13];
    char stringdata16[12];
    char stringdata17[14];
    char stringdata18[12];
    char stringdata19[19];
    char stringdata20[15];
    char stringdata21[14];
    char stringdata22[12];
    char stringdata23[21];
    char stringdata24[19];
    char stringdata25[5];
    char stringdata26[3];
    char stringdata27[10];
    char stringdata28[8];
    char stringdata29[17];
    char stringdata30[18];
    char stringdata31[9];
    char stringdata32[5];
    char stringdata33[10];
    char stringdata34[12];
    char stringdata35[8];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSWebRTCENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSWebRTCENDCLASS_t qt_meta_stringdata_CLASSWebRTCENDCLASS = {
    {
        QT_MOC_LITERAL(0, 6),  // "WebRTC"
        QT_MOC_LITERAL(7, 17),  // "openedDataChannel"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 6),  // "peerId"
        QT_MOC_LITERAL(33, 17),  // "closedDataChannel"
        QT_MOC_LITERAL(51, 13),  // "incomingTrack"
        QT_MOC_LITERAL(65, 9),  // "trackData"
        QT_MOC_LITERAL(75, 25),  // "localDescriptionGenerated"
        QT_MOC_LITERAL(101, 6),  // "peerID"
        QT_MOC_LITERAL(108, 3),  // "sdp"
        QT_MOC_LITERAL(112, 23),  // "localCandidateGenerated"
        QT_MOC_LITERAL(136, 9),  // "candidate"
        QT_MOC_LITERAL(146, 6),  // "sdpMid"
        QT_MOC_LITERAL(153, 16),  // "isOffererChanged"
        QT_MOC_LITERAL(170, 18),  // "gatheringCompleted"
        QT_MOC_LITERAL(189, 12),  // "offerIsReady"
        QT_MOC_LITERAL(202, 11),  // "description"
        QT_MOC_LITERAL(214, 13),  // "answerIsReady"
        QT_MOC_LITERAL(228, 11),  // "ssrcChanged"
        QT_MOC_LITERAL(240, 18),  // "payloadTypeChanged"
        QT_MOC_LITERAL(259, 14),  // "bitRateChanged"
        QT_MOC_LITERAL(274, 13),  // "peerConnected"
        QT_MOC_LITERAL(288, 11),  // "trackOpened"
        QT_MOC_LITERAL(300, 20),  // "setRemoteDescription"
        QT_MOC_LITERAL(321, 18),  // "setRemoteCandidate"
        QT_MOC_LITERAL(340, 4),  // "init"
        QT_MOC_LITERAL(345, 2),  // "id"
        QT_MOC_LITERAL(348, 9),  // "isOfferer"
        QT_MOC_LITERAL(358, 7),  // "addPeer"
        QT_MOC_LITERAL(366, 16),  // "generateOfferSDP"
        QT_MOC_LITERAL(383, 17),  // "generateAnswerSDP"
        QT_MOC_LITERAL(401, 8),  // "addTrack"
        QT_MOC_LITERAL(410, 4),  // "ssrc"
        QT_MOC_LITERAL(415, 9),  // "rtc::SSRC"
        QT_MOC_LITERAL(425, 11),  // "payloadType"
        QT_MOC_LITERAL(437, 7)   // "bitRate"
    },
    "WebRTC",
    "openedDataChannel",
    "",
    "peerId",
    "closedDataChannel",
    "incomingTrack",
    "trackData",
    "localDescriptionGenerated",
    "peerID",
    "sdp",
    "localCandidateGenerated",
    "candidate",
    "sdpMid",
    "isOffererChanged",
    "gatheringCompleted",
    "offerIsReady",
    "description",
    "answerIsReady",
    "ssrcChanged",
    "payloadTypeChanged",
    "bitRateChanged",
    "peerConnected",
    "trackOpened",
    "setRemoteDescription",
    "setRemoteCandidate",
    "init",
    "id",
    "isOfferer",
    "addPeer",
    "generateOfferSDP",
    "generateAnswerSDP",
    "addTrack",
    "ssrc",
    "rtc::SSRC",
    "payloadType",
    "bitRate"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSWebRTCENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       4,  226, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      14,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,  146,    2, 0x06,    5 /* Public */,
       4,    1,  149,    2, 0x06,    7 /* Public */,
       5,    2,  152,    2, 0x06,    9 /* Public */,
       7,    2,  157,    2, 0x06,   12 /* Public */,
      10,    3,  162,    2, 0x06,   15 /* Public */,
      13,    0,  169,    2, 0x06,   19 /* Public */,
      14,    1,  170,    2, 0x06,   20 /* Public */,
      15,    2,  173,    2, 0x06,   22 /* Public */,
      17,    2,  178,    2, 0x06,   25 /* Public */,
      18,    0,  183,    2, 0x06,   28 /* Public */,
      19,    0,  184,    2, 0x06,   29 /* Public */,
      20,    0,  185,    2, 0x06,   30 /* Public */,
      21,    1,  186,    2, 0x06,   31 /* Public */,
      22,    1,  189,    2, 0x06,   33 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      23,    2,  192,    2, 0x0a,   35 /* Public */,
      24,    3,  197,    2, 0x0a,   38 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      25,    2,  204,    2, 0x02,   42 /* Public */,
      25,    1,  209,    2, 0x22,   45 /* Public | MethodCloned */,
      28,    1,  212,    2, 0x02,   47 /* Public */,
      29,    1,  215,    2, 0x02,   49 /* Public */,
      30,    1,  218,    2, 0x02,   51 /* Public */,
      31,    2,  221,    2, 0x02,   53 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QByteArray,    3,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    8,   11,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,   16,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    8,   11,   12,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   26,   27,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString, QMetaType::QByteArray,    3,    6,

 // properties: name, type, flags
      27, QMetaType::Bool, 0x00015907, uint(5), 0,
      32, 0x80000000 | 33, 0x0001590f, uint(9), 0,
      34, QMetaType::Int, 0x00015907, uint(10), 0,
      35, QMetaType::Int, 0x00015907, uint(11), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject WebRTC::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_CLASSWebRTCENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSWebRTCENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSWebRTCENDCLASS_t,
        // property 'isOfferer'
        QtPrivate::TypeAndForceComplete<bool, std::true_type>,
        // property 'ssrc'
        QtPrivate::TypeAndForceComplete<rtc::SSRC, std::true_type>,
        // property 'payloadType'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // property 'bitRate'
        QtPrivate::TypeAndForceComplete<int, std::true_type>,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<WebRTC, std::true_type>,
        // method 'openedDataChannel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'closedDataChannel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'incomingTrack'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'localDescriptionGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'localCandidateGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'isOffererChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'gatheringCompleted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'offerIsReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'answerIsReady'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'ssrcChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'payloadTypeChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'bitRateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'peerConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'trackOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setRemoteDescription'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'setRemoteCandidate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'init'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'init'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'addPeer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'generateOfferSDP'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'generateAnswerSDP'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'addTrack'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>
    >,
    nullptr
} };

void WebRTC::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WebRTC *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->openedDataChannel((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->closedDataChannel((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->incomingTrack((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 3: _t->localDescriptionGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->localCandidateGenerated((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 5: _t->isOffererChanged(); break;
        case 6: _t->gatheringCompleted((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->offerIsReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->answerIsReady((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->ssrcChanged(); break;
        case 10: _t->payloadTypeChanged(); break;
        case 11: _t->bitRateChanged(); break;
        case 12: _t->peerConnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->trackOpened((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->setRemoteDescription((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 15: _t->setRemoteCandidate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 16: _t->init((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 17: _t->init((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: _t->addPeer((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->generateOfferSDP((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 20: _t->generateAnswerSDP((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 21: _t->addTrack((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (WebRTC::*)(const QString & );
            if (_t _q_method = &WebRTC::openedDataChannel; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & );
            if (_t _q_method = &WebRTC::closedDataChannel; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & , const QByteArray & );
            if (_t _q_method = &WebRTC::incomingTrack; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & , const QString & );
            if (_t _q_method = &WebRTC::localDescriptionGenerated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & , const QString & , const QString & );
            if (_t _q_method = &WebRTC::localCandidateGenerated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)();
            if (_t _q_method = &WebRTC::isOffererChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & );
            if (_t _q_method = &WebRTC::gatheringCompleted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & , const QString & );
            if (_t _q_method = &WebRTC::offerIsReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & , const QString & );
            if (_t _q_method = &WebRTC::answerIsReady; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)();
            if (_t _q_method = &WebRTC::ssrcChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)();
            if (_t _q_method = &WebRTC::payloadTypeChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 10;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)();
            if (_t _q_method = &WebRTC::bitRateChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 11;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & );
            if (_t _q_method = &WebRTC::peerConnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 12;
                return;
            }
        }
        {
            using _t = void (WebRTC::*)(const QString & );
            if (_t _q_method = &WebRTC::trackOpened; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 13;
                return;
            }
        }
    }else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<WebRTC *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isOfferer(); break;
        case 1: *reinterpret_cast< rtc::SSRC*>(_v) = _t->ssrc(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->payloadType(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->bitRate(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<WebRTC *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setIsOfferer(*reinterpret_cast< bool*>(_v)); break;
        case 1: _t->setSsrc(*reinterpret_cast< rtc::SSRC*>(_v)); break;
        case 2: _t->setPayloadType(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setBitRate(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
        auto *_t = static_cast<WebRTC *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->resetIsOfferer(); break;
        case 1: _t->resetSsrc(); break;
        case 2: _t->resetPayloadType(); break;
        case 3: _t->resetBitRate(); break;
        default: break;
        }
    } else if (_c == QMetaObject::BindableProperty) {
    }
}

const QMetaObject *WebRTC::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WebRTC::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSWebRTCENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WebRTC::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
    }else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void WebRTC::openedDataChannel(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void WebRTC::closedDataChannel(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WebRTC::incomingTrack(const QString & _t1, const QByteArray & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void WebRTC::localDescriptionGenerated(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void WebRTC::localCandidateGenerated(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void WebRTC::isOffererChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void WebRTC::gatheringCompleted(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void WebRTC::offerIsReady(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void WebRTC::answerIsReady(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void WebRTC::ssrcChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void WebRTC::payloadTypeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void WebRTC::bitRateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void WebRTC::peerConnected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 12, _a);
}

// SIGNAL 13
void WebRTC::trackOpened(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 13, _a);
}
QT_WARNING_POP
