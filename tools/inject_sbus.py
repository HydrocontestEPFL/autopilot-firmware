#!/usr/bin/env python3
"""
Displays a GUI to inject SBUS messages over the given UART port.
"""

import argparse
import sys
import socket
import rpc
import messages
from PyQt5.QtWidgets import *
from PyQt5.QtCore import pyqtSlot, pyqtSignal, QThread, QTimer, Qt
from functools import partial

TOPIC_NAME = "sbus"
CHANNELS = [("Throttle", 3), ("Roll", 0), ("Lift", 2), ("Rudder", 1),
            ("Arming", 5), ("Mode selection", 4)]


class SBusApp(QMainWindow):
    def __init__(self, send_values):
        super().__init__()
        self.setWindowTitle('SBUS Simulator')
        self.channelValues = dict()
        self.send_values = send_values
        outer_box = QVBoxLayout()
        for channel, index in CHANNELS:
            box = QHBoxLayout()
            slider = QSlider(Qt.Horizontal)

            channel = "{} (#{})".format(channel, index)
            label = QLabel(channel)

            slider.setTickInterval(20)
            slider.setTickPosition(3)

            slider.valueChanged.connect(partial(self._slider_changed, i=index))
            slider.setValue(50)

            box.addWidget(label)
            box.addWidget(slider)

            w = QWidget()
            w.setLayout(box)
            outer_box.addWidget(w)

        hbox_widget = QWidget()
        hbox_widget.setLayout(outer_box)
        self.setCentralWidget(hbox_widget)

        self.timer = QTimer()
        self.timer.timeout.connect(self._timer_timeout)
        self.timer.start(100)

        self.show()

    @pyqtSlot(int)
    def _slider_changed(self, v, i):
        v = int(v * 1000 / 100) # map it to useful sbus value
        self.channelValues[i] = v

    @pyqtSlot()
    def _timer_timeout(self):
        self.send_values(self.channelValues)


def udp_sender(target):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def udp_send(values):
        msg = messages.SBUSPacket()
        msg.channels.extend([0] * 16)
        msg.channel16 = False
        msg.channel17 = False
        msg.frame_lost = False
        msg.failsafe = False

        for i, value in values.items():
            msg.channels[i] = value

        packet = rpc.generate_topic_injection_packet(TOPIC_NAME, msg)
        sock.sendto(packet, target)

    return udp_send


def serial_sender(port):
    def send(values):
        raise NotImplementedError

    return send


def parse_args():
    parser = argparse.ArgumentParser(description=__doc__)

    def udp_host(s):
        try:
            s = s.split(':')
            return s[0], int(s[1])
        except:
            raise ValueError

    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        "--udp",
        "-u",
        help="Send the data to an UDP host specified in the host:port format.",
        type=udp_host)
    group.add_argument(
        "--serial", "-s", help="Send the data to the given serial port.")

    return parser.parse_args()


def main():
    args = parse_args()
    if args.udp:
        target = udp_sender(args.udp)
    else:
        target = serial_sender(args.serial)
    app = QApplication(sys.argv)
    ex = SBusApp(target)
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
