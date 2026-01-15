from ultralytics import YOLO
import time
import requests
from collections import Counter

#Smart Detection Class to handle stable frame detection and ESP8266 communication
class SmartDetection:
    def __init__(self, model, esp8266_url):
        self.model = model
        self.esp8266_url = esp8266_url

        # Stable frames configuration
        # if the same object is detected 10 times in these many frames, we consider it stable
        self.STABLE_FRAMES = 10 
        self.current_label = None
        self.frame_count = 0

        # Cooldown configuration
        self.COOLDOWN_SEC = 10
        self.last_send_time = 0

        # Class mapping to messages
        self.class_to_message = {
            "Windel": "restmull",
            "Maske": "restmull",
            "Zahnbuerste": "restmull",
            "Bio": "bio",
            "Verbundkarton": "gelbersack",
            "Metall": "gelbersack",
            "Plastik": "gelbersack",
            "Pappe": "papier",
            "Papier": "papier"
        }

        # Class ID to name mapping
        self.class_mapping = {
            0: "Windel",
            1: "Bio",
            2: "Verbundkarton",
            3: "Pappe",
            4: "Maske",
            5: "Zahnbuerste",
            6: "Plastik",
            7: "Metall",
            8: "Papier"
        }

    def process(self, frame):
        """Run YOLO + stable-frame logic on ONE frame"""

        results = self.model(frame, imgsz=320, conf=0.5)
        annotated = results[0].plot()

        # Extract detected classes
        if results[0].boxes is not None and len(results[0].boxes) > 0:
            class_counts = Counter()

            for box in results[0].boxes:
                class_id = int(box.cls[0])
                class_name = self.class_mapping.get(class_id)
                if class_name:
                    class_counts[class_name] += 1

            if class_counts:
                detected_class = class_counts.most_common(1)[0][0]
                self._process_detection(detected_class)
        else:
            # Reset if detection lost
            self.current_label = None
            self.frame_count = 0

        return annotated

    def _process_detection(self, label):
        """Stable frames logic"""
        if label == self.current_label:
            self.frame_count += 1
        else:
            self.current_label = label
            self.frame_count = 1

        if self.frame_count >= self.STABLE_FRAMES:
            now = time.time()
            if now - self.last_send_time > self.COOLDOWN_SEC:
                self._send_message(label)
                self.last_send_time = now

            self.current_label = None
            self.frame_count = 0

    # Send message to ESP8266
    def _send_message(self, class_name):
        message = self.class_to_message.get(class_name)
        if not message:
            return

        try:
            response = requests.post(
                self.esp8266_url,
                data=message,
                headers={"Content-Type": "text/plain"},
                timeout=2
            )
            print(f"✓ Sent '{message}' (Class: {class_name}) | Status: {response.status_code}")
        except Exception as e:
            print(f"✗ ESP send failed: {e}")