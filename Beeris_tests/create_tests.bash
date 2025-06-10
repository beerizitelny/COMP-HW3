#!/bin/bash

# Script to create all test files for semantic analysis

# Test 13: UnexpectedBreak - break outside while loop
cat > t13.in << 'EOF'
void main() {
    break;
}
EOF

cat > t13.out << 'EOF'
line 2: unexpected break statement
EOF

# Test 14: UnexpectedContinue - continue outside while loop
cat > t14.in << 'EOF'
void main() {
    continue;
}
EOF

cat > t14.out << 'EOF'
line 2: unexpected continue statement
EOF

# Test 15: UnexpectedBreak in if statement (not in while)
cat > t15.in << 'EOF'
void main() {
    if (true) {
        break;
    }
}
EOF

cat > t15.out << 'EOF'
line 3: unexpected break statement
EOF

# Test 16: Valid break in while loop (should pass)
cat > t16.in << 'EOF'
void main() {
    while (true) {
        break;
    }
}
EOF

cat > t16.out << 'EOF'
---begin global scope---
print (string) -> void
printi (int) -> void
main () -> void
  ---begin scope---
    ---begin scope---
    ---end scope---
  ---end scope---
---end global scope---
EOF

# Test 17: Return with expression in void function
cat > t17.in << 'EOF'
void main() {
    return 5;
}
EOF

cat > t17.out << 'EOF'
line 2: type mismatch
EOF

# Test 18: Return without expression in non-void function  
cat > t18.in << 'EOF'
int foo() {
    return;
}

void main() {
}
EOF

cat > t18.out << 'EOF'
line 2: type mismatch
EOF

# Test 19: Valid return with expression in int function
cat > t19.in << 'EOF'
int foo() {
    return 5;
}

void main() {
}
EOF

cat > t19.out << 'EOF'
---begin global scope---
print (string) -> void
printi (int) -> void
foo () -> int
main () -> void
  ---begin scope---
  ---end scope---
  ---begin scope---
  ---end scope---
---end global scope---
EOF

# Test 20: ByteTooLarge - byte literal > 255
cat > t20.in << 'EOF'
void main() {
    byte b = 256B;
}
EOF

cat > t20.out << 'EOF'
line 2: byte value 256 out of range
EOF

# Test 21: Valid byte literal (should pass)
cat > t21.in << 'EOF'
void main() {
    byte b = 255B;
}
EOF

cat > t21.out << 'EOF'
---begin global scope---
print (string) -> void
printi (int) -> void
main () -> void
  ---begin scope---
b byte 0
  ---end scope---
---end global scope---
EOF

# Test 22: Missing main function
cat > t22.in << 'EOF'
int foo() {
    return 5;
}
EOF

cat > t22.out << 'EOF'
Program has no 'void main()' function
EOF

# Test 23: main with parameters (invalid)
cat > t23.in << 'EOF'
void main(int x) {
}
EOF

cat > t23.out << 'EOF'
Program has no 'void main()' function
EOF

# Test 24: main with wrong return type
cat > t24.in << 'EOF'
int main() {
    return 0;
}
EOF

cat > t24.out << 'EOF'
Program has no 'void main()' function
EOF

# Test 25: Invalid assignment to array
cat > t25.in << 'EOF'
void main() {
    int arr[5];
    arr = 10;
}
EOF

cat > t25.out << 'EOF'
line 3: invalid assignment to array arr
EOF

# Test 26: Invalid assignment from array (type mismatch)
cat > t26.in << 'EOF'
void main() {
    int arr[5];
    int x;
    x = arr;
}
EOF

cat > t26.out << 'EOF'
line 4: type mismatch
EOF

# Test 27: Array as function parameter (invalid)
cat > t27.in << 'EOF'
void foo(int arr[5]) {
}

void main() {
}
EOF

cat > t27.out << 'EOF'
line 1: type mismatch
EOF

# Test 28: Array as return type (invalid)
cat > t28.in << 'EOF'
int[5] foo() {
    int arr[5];
    return arr;
}

void main() {
}
EOF

cat > t28.out << 'EOF'
line 1: type mismatch
EOF

# Test 29: Multiple breaks/continues in nested while loops (valid)
cat > t29.in << 'EOF'
void main() {
    while (true) {
        while (false) {
            break;
            continue;
        }
        break;
    }
}
EOF

cat > t29.out << 'EOF'
---begin global scope---
print (string) -> void
printi (int) -> void
main () -> void
  ---begin scope---
    ---begin scope---
      ---begin scope---
      ---end scope---
    ---end scope---
  ---end scope---
---end global scope---
EOF

# Test 30: Continue in if inside while (valid)
cat > t30.in << 'EOF'
void main() {
    while (true) {
        if (true) {
            continue;
        }
    }
}
EOF

cat > t30.out << 'EOF'
---begin global scope---
print (string) -> void
printi (int) -> void
main () -> void
  ---begin scope---
    ---begin scope---
      ---begin scope---
      ---end scope---
    ---end scope---
  ---end scope---
---end global scope---
EOF

# Test 31: ByteTooLarge with edge case (exactly 256)
cat > t31.in << 'EOF'
void main() {
    byte b = 256B;
}
EOF

cat > t31.out << 'EOF'
line 2: byte value 256 out of range
EOF

# Test 32: Multiple main functions (invalid)
cat > t32.in << 'EOF'
void main() {
}

void main() {
}
EOF

cat > t32.out << 'EOF'
line 4: symbol main is already defined
EOF

echo "All test files created successfully!"
echo "Created files: t13.in through t32.out (40 files total)"