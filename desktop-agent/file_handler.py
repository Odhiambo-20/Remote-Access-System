import os
import json
import hashlib
import time
from pathlib import Path

class FileHandler:
    def __init__(self, share_base_url="http://127.0.0.1:8080/shared"):
        self.share_base_url = share_base_url
        self.shared_files = {}  # Store shared file info
        
    def browse_directory(self, path):
        """Browse a directory and return file list"""
        try:
            path_obj = Path(path)
            
            if not path_obj.exists():
                return {"error": "Path does not exist"}
            
            if not path_obj.is_dir():
                return {"error": "Path is not a directory"}
            
            files = []
            for item in path_obj.iterdir():
                try:
                    stat = item.stat()
                    files.append({
                        "name": item.name,
                        "path": str(item),
                        "type": "folder" if item.is_dir() else self._get_file_type(item.name),
                        "size": stat.st_size if item.is_file() else 0,
                        "is_directory": item.is_dir()
                    })
                except PermissionError:
                    continue
            
            # Sort: directories first, then files
            files.sort(key=lambda x: (not x["is_directory"], x["name"].lower()))
            
            return {"files": files}
        except Exception as e:
            return {"error": str(e)}
    
    def _get_file_type(self, filename):
        """Determine file type based on extension"""
        ext = Path(filename).suffix.lower()
        
        type_map = {
            '.txt': 'text',
            '.pdf': 'pdf',
            '.doc': 'document',
            '.docx': 'document',
            '.xls': 'spreadsheet',
            '.xlsx': 'spreadsheet',
            '.jpg': 'image',
            '.jpeg': 'image',
            '.png': 'image',
            '.gif': 'image',
            '.mp3': 'audio',
            '.wav': 'audio',
            '.mp4': 'video',
            '.avi': 'video',
            '.zip': 'archive',
            '.rar': 'archive',
            '.tar': 'archive',
            '.gz': 'archive',
            '.py': 'code',
            '.js': 'code',
            '.cpp': 'code',
            '.h': 'code',
            '.java': 'code',
        }
        
        return type_map.get(ext, 'file')
    
    def share_file(self, file_path):
        """Share a file and return shareable URL"""
        try:
            path_obj = Path(file_path)
            
            if not path_obj.exists():
                return {"error": "File does not exist"}
            
            if not path_obj.is_file():
                return {"error": "Path is not a file"}
            
            # Generate unique file ID
            file_id = hashlib.md5(f"{file_path}{time.time()}".encode()).hexdigest()[:12]
            
            # Store file info
            self.shared_files[file_id] = {
                "path": str(path_obj),
                "name": path_obj.name,
                "size": path_obj.stat().st_size,
                "created": time.time()
            }
            
            # Generate URL
            share_url = f"{self.share_base_url}/{file_id}/{path_obj.name}"
            
            return {
                "success": True,
                "url": share_url,
                "file_id": file_id
            }
        except Exception as e:
            return {"error": str(e)}
    
    def get_file_info(self, file_path):
        """Get detailed file information"""
        try:
            path_obj = Path(file_path)
            
            if not path_obj.exists():
                return {"error": "Path does not exist"}
            
            stat = path_obj.stat()
            
            return {
                "name": path_obj.name,
                "path": str(path_obj),
                "size": stat.st_size,
                "type": "folder" if path_obj.is_dir() else self._get_file_type(path_obj.name),
                "is_directory": path_obj.is_dir(),
                "modified": stat.st_mtime,
                "created": stat.st_ctime
            }
        except Exception as e:
            return {"error": str(e)}
    
    def read_file(self, file_path, max_size=1024*1024):
        """Read file content (for text files)"""
        try:
            path_obj = Path(file_path)
            
            if not path_obj.exists():
                return {"error": "File does not exist"}
            
            if not path_obj.is_file():
                return {"error": "Path is not a file"}
            
            if path_obj.stat().st_size > max_size:
                return {"error": f"File too large (max {max_size} bytes)"}
            
            # Try to read as text
            try:
                with open(path_obj, 'r', encoding='utf-8') as f:
                    content = f.read()
                return {"content": content, "type": "text"}
            except UnicodeDecodeError:
                return {"error": "File is not a text file"}
                
        except Exception as e:
            return {"error": str(e)}
