# SIM800L SMS Sending: Why We Use a Pre-Allocated Buffer

This project uses a **pre-allocated buffer approach** to handle SMS command construction for the SIM800L module. Below is a quick comparison of this approach versus dynamically constructing commands, and why we chose pre-allocation.

## Comparison Table

| Aspect           | Pre-Allocated Buffer                     | Dynamic Command Construction           | Reason for Choosing Pre-Allocated  |
|------------------|------------------------------------------|----------------------------------------|------------------------------------|
| **Memory Usage** | Fixed, predictable                       | Slightly less but variable             | Stability is key in embedded systems |
| **Safety**       | Low risk of overflow                     | Higher risk with variable data         | Pre-allocation minimizes errors    |
| **Performance**  | Faster, no repeated allocation           | Minor overhead with each function call | Efficient for frequent SMS operations |
| **Readability**  | Simple, dedicated buffer                 | Requires more handling for each command | Clearer for maintenance            |

### Conclusion

Using a **pre-allocated buffer** keeps memory usage predictable and the code safer, making it an ideal choice for embedded systems like STM32 where resources are limited.
