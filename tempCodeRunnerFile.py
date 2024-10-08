import json
from datetime import datetime
from prettytable import PrettyTable

class Booking:
    def __init__(self, booking_id, customer_name, room_type, num_pax, check_in, check_out, payment_method, pin):
        self.booking_id = booking_id
        self.customer_name = customer_name
        self.room_type = room_type
        self.num_pax = num_pax
        self.check_in = check_in
        self.check_out = check_out
        self.payment_method = payment_method
        self.pin = pin

class BookingSystem:
    def __init__(self, booking_file='bookings.json'):
        self.booking_file = booking_file
        self.bookings = []
        self.allowed_pins = [987287, 987249, 987177, 987201]
        self.load_bookings()
        self.next_booking_id = self.get_next_booking_id()

    def get_next_booking_id(self):
        if not self.bookings:
            return 1
        return max(booking.booking_id for booking in self.bookings) + 1

    def load_bookings(self):
        try:
            with open(self.booking_file, 'r') as f:
                bookings_data = json.load(f)
                self.bookings = [Booking(**data) for data in bookings_data]
        except FileNotFoundError:
            self.bookings = []

    def save_bookings(self):
        with open(self.booking_file, 'w') as f:
            json.dump([booking.__dict__ for booking in self.bookings], f)

    def login(self):
        attempts = 3
        while attempts > 0:
            pin = input("Enter your PIN: ")
            if pin.isdigit() and int(pin) in self.allowed_pins:
                print("Login successful.")
                return int(pin)
            else:
                attempts -= 1
                print(f"Invalid PIN. {attempts} attempts remaining.")
        print("Access denied.")
        return None

    def create_booking(self, pin):
        if len(self.bookings) >= 4:
            print("\nBooking limit reached. Cannot create more than 4 bookings.")
            return
        
        customer_name = input("Enter customer name: ")
        while not customer_name.isalpha():
            print("Invalid input. Name should contain only letters.")
            customer_name = input("Enter customer name: ")
        
        room_type = input("Enter room type [standard, deluxe, suite]: ")
        while not room_type.isalpha():
            print("Invalid input. Room type should contain only letters.")
            room_type = input("Enter room type [standard, deluxe, suite]: ")

        num_pax = input("Enter number of pax (numeric only): ")
        while not num_pax.isdigit():
            print("Invalid input. Number of pax should be a numeric value.")
            num_pax = input("Enter number of pax: ")
        
        check_in = input("Enter check-in date (mm-dd-yyyy): ")
        check_out = input("Enter check-out date (mm-dd-yyyy): ")
        payment_method = input("Enter payment method: ")

        booking = Booking(self.next_booking_id, customer_name, room_type, int(num_pax), check_in, check_out, payment_method, pin)
        self.bookings.append(booking)
        print(f"\nBooking successfully created for {customer_name} in {room_type}.")
        self.next_booking_id += 1
        self.save_bookings()

    def view_bookings(self, pin):
        user_bookings = [booking for booking in self.bookings if booking.pin == pin]

        if not user_bookings:
            print("\nNo bookings found for this PIN.")
            return

        table = PrettyTable()
        table.field_names = ["Booking ID", "Customer Name", "Room Type", "Number of Pax", "Check-in", "Check-out", "Payment Method"]

        for booking in user_bookings:
            table.add_row([booking.booking_id, booking.customer_name, booking.room_type, booking.num_pax, booking.check_in, booking.check_out, booking.payment_method])

        print("\nYour Bookings:")
        print(table)
        input("Press enter to return to the main menu.")

    def update_booking(self, pin):
        try:
            booking_id = int(input("Enter booking ID to update: "))
            booking = next((b for b in self.bookings if b.booking_id == booking_id and b.pin == pin), None)

            if booking is None:
                print("Booking ID not found or you do not have permission to update this booking.")
                return

            print("\nCurrent details:")
            print(f"Customer Name: {booking.customer_name}")
            print(f"Room Type: {booking.room_type}")
            print(f"Number of Pax: {booking.num_pax}")
            print(f"Check-in Date: {booking.check_in}")
            print(f"Check-out Date: {booking.check_out}")
            print(f"Payment Method: {booking.payment_method}")

            new_customer_name = input("Enter new customer name (leave blank to keep current): ")
            while new_customer_name and not new_customer_name.isalpha():
                print("Invalid input. Name should contain only letters.")
                new_customer_name = input("Enter new customer name (leave blank to keep current): ")

            new_room_type = input("Enter new room type (leave blank to keep current): ")
            while new_room_type and not new_room_type.isalpha():
                print("Invalid input. Room type should contain only letters.")
                new_room_type = input("Enter new room type (leave blank to keep current): ")

            new_num_pax = input("Enter new number of pax (leave blank to keep current): ")
            while new_num_pax and not new_num_pax.isdigit():
                print("Invalid input. Number of pax should be numeric.")
                new_num_pax = input("Enter new number of pax (leave blank to keep current): ")

            if new_customer_name:
                booking.customer_name = new_customer_name
            if new_room_type:
                booking.room_type = new_room_type
            if new_num_pax:
                booking.num_pax = int(new_num_pax)
            booking.check_in = input("Enter new check-in date (mm-dd-yyyy, leave blank to keep current): ") or booking.check_in
            booking.check_out = input("Enter new check-out date (mm-dd-yyyy, leave blank to keep current): ") or booking.check_out
            booking.payment_method = input("Enter new payment method (leave blank to keep current): ") or booking.payment_method

            print("\nBooking successfully updated.")
            self.save_bookings()
        except ValueError:
            print("Invalid input! Please enter a valid booking ID.")

    def delete_booking(self, pin):
        try:
            booking_id = int(input("Enter booking ID to delete: "))
            booking = next((b for b in self.bookings if b.booking_id == booking_id and b.pin == pin), None)

            if booking is None:
                print("Booking ID not found or you do not have permission to delete this booking.")
                return

            confirmation = input(f"Are you sure you want to delete booking ID {booking_id}? (y/n): ")
            if confirmation.lower() == 'y':
                self.bookings.remove(booking)
                print(f"\nBooking ID {booking_id} has been successfully deleted.")
                self.save_bookings()
            else:
                print("Deletion cancelled.")
        except ValueError:
            print("Invalid input! Please enter a valid booking ID.")

    def run(self):
        pin = self.login()
        if pin is None:
            return

        while True:
            print("\nPlease choose an option:")
            print("1. Create a new booking")
            print("2. View your bookings")
            print("3. Update a booking")
            print("4. Delete a booking")
            print("5. Exit")

            try:
                choice = int(input("Enter your choice (1-5): "))

                if choice == 1:
                    self.create_booking(pin)
                elif choice == 2:
                    self.view_bookings(pin)
                elif choice == 3:
                    self.update_booking(pin)
                elif choice == 4:
                    self.delete_booking(pin)
                elif choice == 5:
                    print("Exiting the program.")
                    break
                else:
                    print("Invalid choice.")
            except ValueError:
                print("Invalid input! Please enter a number between 1 and 5.")

if __name__ == "__main__":
    booking_system = BookingSystem()
    booking_system.run()
