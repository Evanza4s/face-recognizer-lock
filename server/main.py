from flask import Flask, request, jsonify
import cv2
import numpy as np
import face_recognition

app = Flask(__name__)

# Load face encoding dari database (contoh)
known_faces = {
    "user1": face_recognition.face_encodings(face_recognition.load_image_file("user1.jpg"))[0],
    "user2": face_recognition.face_encodings(face_recognition.load_image_file("user2.jpg"))[0]
}

@app.route('/recognize', methods=['POST'])
def recognize():
    image = np.frombuffer(request.data, np.uint8)
    img = cv2.imdecode(image, cv2.IMREAD_COLOR)
    
    face_locations = face_recognition.face_locations(img)
    face_encodings = face_recognition.face_encodings(img, face_locations)

    for face_encoding in face_encodings:
        for name, known_encoding in known_faces.items():
            match = face_recognition.compare_faces([known_encoding], face_encoding)[0]
            if match:
                return jsonify({"status": "recognized", "name": name})
    
    return jsonify({"status": "unknown"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
