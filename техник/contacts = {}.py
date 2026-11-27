contacts = {}

def show_menu():
    print("1 - добавить, 2 - найти, 3 - все, 0 - выход")

def add_contact():
    name = input("Имя: ").strip()
    if not name:
        return
    if name in contacts:
        print("Уже есть!")
        return
    phone = input("Телефон: ").strip()
    if not phone:
        return
    # ... дописать

def find_contact():
    name = input("Имя: ").strip()
    # ... дописать

def show_all():
    # ... дописать

def main():
    while True:
        show_menu()
        choice = input("> ").strip()
        if choice == "1":
            add_contact()
        elif choice == "2":
            find_contact()
        elif choice == "3":
            show_all()
        elif choice == "0":
            print("Пока!")
            break
        else:
            print("Не понял.")

if __name__ == "__main__":
    main()