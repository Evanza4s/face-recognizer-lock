import Image from "next/image";
import React, { useState, useEffect } from "react";

export default function Home() {
  const [name, setName] = useState("");
    const [file, setFile] = useState<File | null>(null);
    const [users, setUsers] = useState<{ id: number; name: string; image_path: string }[]>([]);

    useEffect(() => {
        fetchUsers();
    }, []);

    const fetchUsers = async () => {
        const res = await fetch("http://localhost:8000/users");
        const data = await res.json();
        setUsers(data);
    };

    const uploadFace = async () => {
        const formData = new FormData();
        formData.append("name", name);
        if (file) {
            formData.append("file", file);
        } else {
            alert("Please select a file to upload.");
            return;
        }

        const res = await fetch("http://localhost:8000/upload/", {
            method: "POST",
            body: formData,
        });

        if (res.ok) {
            alert("Wajah berhasil diunggah!");
            fetchUsers();
        } else {
            alert("Gagal mengunggah wajah.");
        }
    };

    return (
        <div className="min-h-screen bg-gray-100 flex flex-col items-center p-6">
            <h1 className="text-3xl font-bold text-gray-800 mb-6">Sistem Pengenalan Wajah</h1>
            <div className="bg-white p-6 rounded-lg shadow-md w-full max-w-md">
                <h2 className="text-xl font-semibold mb-4">Upload Wajah</h2>
                <input 
                    type="text" 
                    placeholder="Nama" 
                    className="border p-2 w-full mb-3 rounded" 
                    onChange={(e) => setName(e.target.value)}
                />
                <input 
                    type="file" 
                    className="border p-2 w-full mb-3 rounded" 
                    onChange={(e) => {
                        if (e.target.files) {
                            setFile(e.target.files[0]);
                        }
                    }}
                />
                <button 
                    onClick={uploadFace} 
                    className="bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600 w-full"
                >Upload</button>
            </div>
            <div className="mt-6 w-full max-w-md">
                <h2 className="text-xl font-semibold mb-4">Daftar Pengguna</h2>
                <div className="bg-white p-4 rounded-lg shadow-md">
                    {users.length === 0 ? (
                        <p className="text-gray-500">Belum ada pengguna terdaftar</p>
                    ) : (
                        users.map((user) => (
                            <div key={user.id} className="flex items-center space-x-4 border-b py-2">
                                <Image 
                                    src={`http://localhost:8000/${user.image_path}`} 
                                    alt={user.name} 
                                    className="w-12 h-12 rounded-full object-cover"
                                />
                                <span className="text-gray-800 font-medium">{user.name}</span>
                            </div>
                        ))
                    )}
                </div>
            </div>
        </div>
    );
}
