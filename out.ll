; ModuleID = 'MyModule'
source_filename = "MyModule"

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  store i32 14, ptr %0, align 4
  %1 = call i32 @exit(i32 14)
  ret i32 0
}

declare i32 @exit()
