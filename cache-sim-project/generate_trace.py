import random
import sys

def generate_random_addresses(num_outputs, start_addr_hex, end_addr_hex):
    
    if num_outputs <= 0:
        return []

    try:
        # Convert the hexadecimal string inputs to integers
        start_addr_int = int(start_addr_hex, 16)
        end_addr_int = int(end_addr_hex, 16)
    except ValueError:
        print("Error: Invalid hexadecimal address format. Ensure addresses start with '0x'.", file=sys.stderr)
        return None

    if start_addr_int >= end_addr_int:
        print("Error: Start address must be less than end address.", file=sys.stderr)
        return None

    generated_addresses_int = []
    for _ in range(num_outputs):
        random_addr = random.randint(start_addr_int, end_addr_int)
        generated_addresses_int.append(random_addr)

    generated_addresses_hex = [f"0x{addr:X}" for addr in generated_addresses_int]
    
    return generated_addresses_hex

if __name__ == "__main__":
    print("Generates a trace with no spatial or temporal locality.\n")
    
    try:
        num_outputs_str = input("Enter the number of addresses to generate (e.g., 100): ")
        num_outputs = int(num_outputs_str)

        start_addr_hex = input("Enter the start address (e.g., 0x1000): ")
        end_addr_hex = input("Enter the end address (e.g., 0x8000): ")
        
        if not start_addr_hex.startswith("0x") or not end_addr_hex.startswith("0x"):
            raise ValueError("Addresses must start with '0x'")

        generated_list = generate_random_addresses(num_outputs, start_addr_hex, end_addr_hex)

        if generated_list is not None:
            output_str = ""
            line_len = 0
            max_line_len = 85 

            for i, addr in enumerate(generated_list):
                 output_str += addr
                 line_len += len(addr)
                 if i < len(generated_list) - 1: # If not the last element
                     output_str += ", "
                     line_len += 2
                     if line_len > max_line_len:
                          output_str += "\n"
                          line_len = 0
                 
            print("\n--- Generated Address Trace (copy and paste into simulator) ---\n")
            print(output_str)

    except ValueError as e:
        print(f"\nError: Invalid input. Please enter integers and valid hex addresses. Details: {e}", file=sys.stderr)
    except Exception as e:
        print(f"\nAn unexpected error occurred: {e}", file=sys.stderr)