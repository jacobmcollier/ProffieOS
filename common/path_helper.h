#ifndef COMMON_PATH_HELPER_H
#define COMMON_PATH_HELPER_H

struct PathHelper {
  void Append(const char* name) {
    if (strlen(path_) && path_[strlen(path_)-1] != '/') {
      strcat(path_, "/");
    }
    strcat(path_, name);
  }
  void Append(const char* name, const char *ext) {
    if (strlen(path_) && path_[strlen(path_)-1] != '/') {
      strcat(path_, "/");
    }
    strcat(path_, name);
    strcat(path_, ".");
    strcat(path_, ext);
  }
  void Set(const char* path) {
    strcpy(path_, path ? path : "");
  }
  void Set(const char* path, const char* p2) {
    Set(path);
    Append(p2);
  }
  void Set(const char* path, const char* p2, const char* ext) {
    Set(path);
    Append(p2, ext);
  }
  explicit PathHelper(const char* path) {
    Set(path);
  }
  explicit PathHelper(const char* path, const char* p2) {
    Set(path, p2);
  }
  explicit PathHelper(const char* path, const char* p2, const char* ext) {
    Set(path, p2, ext);
  }
  bool IsRoot() const {
    if (path_[0] == 0) return true;
    if (path_[0] == '/' && path_[1] == 0) return true;
    return false;
  }
  bool Dirname() {
    char *p = strrchr(path_, '/');
    if (p) {
      *p = 0;
      return !IsRoot();
    }
    return false;
  }
  void UndoDirname() {
    path_[strlen(path_)] = '/';
  }
  operator const char*() const { return path_; }
#ifdef F_MAXPATH  
  char path_[F_MAXPATH];
#else
  char path_[128];
#endif
};

#endif
