from fastapi import FastAPI, File, UploadFile, HTTPException
from sqlalchemy import create_engine, Column, Integer, String, LargeBinary
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
import numpy as np
import cv2
import face_recognition
import io
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

# Allowed origins (Frontend URLs or clients)
origins = [
    "http://localhost:3000",  # React frontend
    "http://127.0.0.1:5500",  # Local HTML/JS file
    "https://yourfrontend.com"  # Production frontend
]

# Add CORS Middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,  # Allow specified origins
    allow_credentials=True,
    allow_methods=["*"],  # Allow all HTTP methods (GET, POST, etc.)
    allow_headers=["*"],  # Allow all headers
)

# Database Setup
DATABASE_URL = "postgresql://username:password@localhost:5432/nama_database"
engine = create_engine(DATABASE_URL)
SessionLocal = sessionmaker(bind=engine)
Base = declarative_base()

class User(Base):
    __tablename__ = "users"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True)
    face_encoding = Column(LargeBinary)

Base.metadata.create_all(bind=engine)

# Convert encoding to binary
def encode_to_blob(face_encoding):
    return np.array(face_encoding).tobytes()

def decode_from_blob(blob):
    return np.frombuffer(blob, dtype=np.float64)

@app.post("/upload/")
async def upload_face(name: str, file: UploadFile = File(...)):
    image = np.array(bytearray(file.file.read()), dtype=np.uint8)
    image = cv2.imdecode(image, cv2.IMREAD_COLOR)
    
    face_locations = face_recognition.face_locations(image)
    if not face_locations:
        raise HTTPException(status_code=400, detail="No face detected")

    face_encoding = face_recognition.face_encodings(image, face_locations)[0]
    
    db = SessionLocal()
    new_user = User(name=name, face_encoding=encode_to_blob(face_encoding))
    db.add(new_user)
    db.commit()
    db.close()

    return {"message": f"User {name} added successfully"}

@app.post("/recognize/")
async def recognize_face(file: UploadFile = File(...)):
    image = np.array(bytearray(file.file.read()), dtype=np.uint8)
    image = cv2.imdecode(image, cv2.IMREAD_COLOR)
    
    face_locations = face_recognition.face_locations(image)
    if not face_locations:
        raise HTTPException(status_code=400, detail="No face detected")

    face_encoding = face_recognition.face_encodings(image, face_locations)[0]
    
    db = SessionLocal()
    users = db.query(User).all()
    
    for user in users:
        known_encoding = decode_from_blob(user.face_encoding)
        match = face_recognition.compare_faces([known_encoding], face_encoding)[0]
        if match:
            return {"message": "Face recognized", "user": user.name}

    return {"message": "Face not recognized"}
