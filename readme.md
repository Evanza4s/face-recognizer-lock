CREATE TABLE users (
id INTEGER PRIMARY KEY AUTOINCREMENT,
name TEXT NOT NULL,
face_encoding BLOB NOT NULL
);

install cmake

python -m venv env

./env/Script/activate

or

source /env/bin/activate

pip install dlib

pip install fastapi uvicorn sqlalchemy face_recognition opencv-python numpy pillow

uvicorn main:app -port 8000 --reload

<!-- frontend -->

npm i

npm run dev
