import cv2
import mediapipe as mp

# Initialize MediaPipe Face Detection and Hands
mp_face_detection = mp.solutions.face_detection
mp_hands = mp.solutions.hands
mp_draw = mp.solutions.drawing_utils

# Set up Face Detection and Hand Tracking
face_detection = mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.7)
hands = mp_hands.Hands(max_num_hands=2, min_detection_confidence=0.7)

# Function to draw a dotted rectangle around the face
def draw_dotted_rectangle(frame, top_left, bottom_right, color=(255, 0, 0), dot_length=5, spacing=5):
    x1, y1 = top_left
    x2, y2 = bottom_right
    for i in range(x1, x2, dot_length + spacing):
        cv2.line(frame, (i, y1), (i + dot_length, y1), color, 2)
    for i in range(x1, x2, dot_length + spacing):
        cv2.line(frame, (i, y2), (i + dot_length, y2), color, 2)
    for i in range(y1, y2, dot_length + spacing):
        cv2.line(frame, (x1, i), (x1, i + dot_length), color, 2)
    for i in range(y1, y2, dot_length + spacing):
        cv2.line(frame, (x2, i), (x2, i + dot_length), color, 2)

# Function to count fingers
def count_fingers(hand_landmarks):
    landmarks = hand_landmarks.landmark
    fingers_up = [
        (landmarks[mp_hands.HandLandmark.THUMB_TIP].x > landmarks[mp_hands.HandLandmark.THUMB_IP].x),
        (landmarks[mp_hands.HandLandmark.INDEX_FINGER_TIP].y < landmarks[mp_hands.HandLandmark.INDEX_FINGER_DIP].y),
        (landmarks[mp_hands.HandLandmark.MIDDLE_FINGER_TIP].y < landmarks[mp_hands.HandLandmark.MIDDLE_FINGER_DIP].y),
        (landmarks[mp_hands.HandLandmark.RING_FINGER_TIP].y < landmarks[mp_hands.HandLandmark.RING_FINGER_DIP].y),
        (landmarks[mp_hands.HandLandmark.PINKY_TIP].y < landmarks[mp_hands.HandLandmark.PINKY_DIP].y)
    ]
    return sum(fingers_up)

# Function to check if the index finger is up and if it overlaps with the button
def is_index_finger_up_and_button_pressed(hand_landmarks, button_rect, frame_height, frame_width):
    landmarks = hand_landmarks.landmark
    index_finger_up = (landmarks[mp_hands.HandLandmark.INDEX_FINGER_TIP].y < landmarks[mp_hands.HandLandmark.INDEX_FINGER_DIP].y)

    # Get button coordinates
    ix = int(landmarks[mp_hands.HandLandmark.INDEX_FINGER_TIP].x * frame_width)
    iy = int(landmarks[mp_hands.HandLandmark.INDEX_FINGER_TIP].y * frame_height)

    # Check if the index finger is above the button and within the button boundaries
    if index_finger_up and button_rect[0] < ix < button_rect[0] + button_rect[2] and button_rect[1] < iy < button_rect[1] + button_rect[3]:
        return True
    return False

# Start video capture
cap = cv2.VideoCapture(0)

# Button properties
button_w, button_h = 150, 60    # Size of the button
button_color = (50, 150, 50)       # Button color (dark green)
button_pressed_color = (0, 100, 200)  # Color when button is pressed (dark blue)

# Main loop
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break
        
    frame_height, frame_width, _ = frame.shape
    screen_res = (frame_width, frame_height)
    
    # Draw button in the center of the frame
    button_x = (frame_width - button_w) // 2
    button_y = (frame_height - button_h) // 2
    cv2.rectangle(frame, (button_x, button_y), (button_x + button_w, button_y + button_h), button_color, -1)
    
    # Draw button border
    cv2.rectangle(frame, (button_x, button_y), (button_x + button_w, button_y + button_h), (0, 0, 0), 2)  # Black border
    
    # Button text properties
    button_text = "Click Me"
    font_scale = 0.7
    text_color = (255, 255, 255)  # White text
    text_thickness = 2
    
    # Get text size for centering
    text_size = cv2.getTextSize(button_text, cv2.FONT_HERSHEY_SIMPLEX, font_scale, text_thickness)[0]
    text_x = button_x + (button_w - text_size[0]) // 2
    text_y = button_y + (button_h + text_size[1]) // 2
    
    cv2.putText(frame, button_text, (text_x, text_y), cv2.FONT_HERSHEY_SIMPLEX, font_scale, text_color, text_thickness)

    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    face_results = face_detection.process(rgb_frame)
    hand_results = hands.process(rgb_frame)

    if face_results.detections:
        for detection in face_results.detections:
            bboxC = detection.location_data.relative_bounding_box
            h, w, _ = frame.shape
            x1, y1 = int(bboxC.xmin * w), int(bboxC.ymin * h)
            x2, y2 = int((bboxC.xmin + bboxC.width) * w), int((bboxC.ymin + bboxC.height) * h)
            draw_dotted_rectangle(frame, (x1, y1), (x2, y2), color=(255, 0, 0))

            if hand_results.multi_hand_landmarks:
                for hand_landmarks in hand_results.multi_hand_landmarks:
                    wrist_x = int(hand_landmarks.landmark[mp_hands.HandLandmark.WRIST].x * w)
                    wrist_y = int(hand_landmarks.landmark[mp_hands.HandLandmark.WRIST].y * h)
                    cv2.line(frame, (x1, y1), (wrist_x, wrist_y), (0, 255, 0), 2)
                    cv2.line(frame, (x2, y1), (wrist_x, wrist_y), (0, 255, 0), 2)
                    cv2.line(frame, (x1, y2), (wrist_x, wrist_y), (0, 255, 0), 2)
                    cv2.line(frame, (x2, y2), (wrist_x, wrist_y), (0, 255, 0), 2)

    # Count and display fingers
    if hand_results.multi_hand_landmarks:
        for hand_landmarks in hand_results.multi_hand_landmarks:
            mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            fingers_count = count_fingers(hand_landmarks)
            cv2.putText(frame, f'Fingers Count: {fingers_count}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)

            # Check if index finger is up and overlaps with button
            if is_index_finger_up_and_button_pressed(hand_landmarks, (button_x, button_y, button_w, button_h), frame_height, frame_width):
                cv2.rectangle(frame, (button_x, button_y), (button_x + button_w, button_y + button_h), button_pressed_color, -1)
                cv2.putText(frame, "Clicked!", (button_x + 10, button_y + 30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
                # Output the value "ILOVEYOU"
                cv2.putText(frame, "ILOVEYOU", (frame_width // 2 - 50, frame_height // 2), cv2.FONT_HERSHEY_SIMPLEX, 2, (255, 0, 0), 3)
                print("ILOVEYOU")
                
    cv2.imshow("Face and Palm Detection with Button", frame)

    if cv2.waitKey(1) & 0xFF == 27:
        break
cap.release()
cv2.destroyAllWindows()
