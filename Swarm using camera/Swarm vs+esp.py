import serial
import time
import cv2
import numpy as np
import math

# Serial setup (if needed)
# SERIAL_PORT = 'COM14'  
# BAUD_RATE = 115200
# ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
# time.sleep(2)

aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
parameters = cv2.aruco.DetectorParameters()

CAMERA_URL = "http://172.20.10.2:8080/video"

cap = cv2.VideoCapture(CAMERA_URL)

bot_speed = 3.0  # speed of bot movement (pixels per frame)
bot_pos = None   # will hold current simulated bot position

def move_towards(current_pos, target_pos, speed):
    direction = target_pos - current_pos
    dist = np.linalg.norm(direction)
    if dist < speed or dist == 0:
        return target_pos
    step = (direction / dist) * speed
    return current_pos + step

while True:
    ret, frame = cap.read()
    if not ret:
        print("Failed to grab frame")
        break

    detector = cv2.aruco.ArucoDetector(aruco_dict, parameters)
    corners, ids, _ = detector.detectMarkers(frame)

    if ids is not None:
        # Find bot marker and package marker
        bot_marker_index = None
        package_marker_index = None

        for i, id_val in enumerate(ids.flatten()):
            if id_val == 1:
                bot_marker_index = i
            elif id_val == 2:
                package_marker_index = i

        if bot_marker_index is not None:
            bot_corners = corners[bot_marker_index][0]
            bot_center = np.mean(bot_corners, axis=0)
            bot_center = np.array(bot_center, dtype=np.float32)
            # Initialize bot_pos at the first detected bot marker center
            if bot_pos is None:s
                bot_pos = bot_center

            # Draw bot marker green
            cv2.polylines(frame, [np.int32(bot_corners)], True, (0, 255, 0), 3)
            cv2.circle(frame, tuple(bot_center.astype(int)), 10, (0, 255, 0), -1)
            cv2.putText(frame, "Bot Marker (1)", tuple(bot_center.astype(int) + np.array([10, 0])),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

        if package_marker_index is not None:
            package_corners = corners[package_marker_index][0]
            package_center = np.mean(package_corners, axis=0)
            package_center = np.array(package_center, dtype=np.float32)

            # Draw package marker blue
            cv2.polylines(frame, [np.int32(package_corners)], True, (255, 0, 0), 3)
            cv2.circle(frame, tuple(package_center.astype(int)), 10, (255, 0, 0), -1)
            cv2.putText(frame, "Package Marker (2)", tuple(package_center.astype(int) + np.array([10, 0])),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 0, 0), 2)

            # If we have bot_pos, move it towards package
            if bot_pos is not None:
                bot_pos = move_towards(bot_pos, package_center, bot_speed)

                # Draw simulated bot position yellow
                cv2.circle(frame, tuple(bot_pos.astype(int)), 15, (0, 255, 255), -1)
                cv2.putText(frame, "Bot Position (Sim)", tuple(bot_pos.astype(int) + np.array([10, 20])),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)

                # Here you can send the bot_pos to your ESP32 over serial
                # For example: ser.write(f"{bot_pos[0]},{bot_pos[1]}\n".encode())

    cv2.imshow("Bot Moving to Package", frame)

    if cv2.waitKey(1) == 27:
        break

cap.release()
cv2.destroyAllWindows()
# ser.close()
