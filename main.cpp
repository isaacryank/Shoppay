#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <mysql.h> // libmysql
#include <sstream> // For stringstream
#include <limits>
#include <algorithm> // For transform

#include <cstdlib> // For system()
#include <tuple>


#undef max

using namespace std;

// Structs
struct User {
    int id;
    string username;
    string email;
    string password;
    string fullName;
    string address;
    int phoneNumber;
    int UserRole; // 0 = Customer, 1 = Seller, 2 = Admin
    bool isApproved; // New member to track approval status for sellers
};

struct CartItem {
    string productID;
    int quantity;
    double unitPrice;
};

struct Product {
    string userID;
    string productID;
    string name;
    string description;
    double price;
    int stockQuantity;
    string categoryID;
};

struct Order {
    string orderID;
    string userID;
    double totalAmount;
    string status;
};

// Global Variables
MYSQL* conn; // Global connection object
string glbStr; // Global string for user identification
vector<CartItem> cart; // Global variable to hold cart items
double totalAmount = 0; // To track thQe total amount during checkout
string currentUserID; // To store the ID of the currently logged-in user
int currentUserRole;  // To store the role of the currently logged-in user (0 = Customer, 1 = Seller, 2 = Admin)


// Function Declarations
void connectToDatabase();
bool executeUpdate(const string& query);
void clearScreen();
void displayBanner();
void mainInterface();
void mainMenu();
void registerUser(); // Declare registerUser  function
void approveSeller();
void displaySellerMenu();
void manageMyProducts();
void addProductSeller();
void updateProductSeller();
void deleteProductSeller();
void viewMyProductsSeller();
void approveCustomerOrders();
void viewSalesReports();
void viewSalesReport(const string& sellerID);
void contactCustomers();
bool loginUser();
bool checkAdmin(const string& user);
void addProduct();
void generateCoupon();
void viewReports();
void displayAdminMenu();
bool applyCoupon(const string& couponCode);
void checkout(const string& userID);
void insertProduct(const string& productName, double price, int stock);
void updateProduct(int productId, const string& productName, double price, int stock);
void deleteProduct(int productId);
void searchProduct(const string& keyword);
void addItemToCart(int quantity, const string& productID);
void updateCartItem(const string& productID, int newQuantity);
void removeCartItem(const string& productID);
string fixedPrice(double price);
void viewCart(const string& userID);
void placeOrder(const Order& order);
void viewOrders(const string& userID);
void updateOrderStatus(const string& orderID, const string& status);
void deleteOrder(const string& orderID);
void displayReceipt(const string& orderID, const string& userID);
void browseProducts(); // Declare browseProducts function
void manageProduct();
void updateProductInterface();
void deleteProductInterface();
void viewProducts();
void searchProductInterface();
void searchProductWithFilters();
void browseProducts(); // Declare browseProducts function
void displaySellerDashboard();
void checkoutTransaction(const string& userID);
bool isCouponValid(const string& couponCode);
void createCoupon(const string& code, double discountPercentage, const string& validFrom, const string& validUntil, double minPurchase, int usageLimit = -1);
void couponManagement();
void updateCoupon(int couponID, const string& validUntil);
void deleteCoupon(int couponID);
void viewCoupons();
void viewWalletBalance(const string& userID);
void viewTransactionHistory(const string& userID, const string& transactionType = "all");

void viewSellerWalletBalance(const string& sellerID);
void monitorAllWalletTransactions();
void updateCommissionRate();
void addLoyaltyPoints(const string& customerID, double totalAmount);
void addAddress(const string& userID);
void displayAddresses(const string& userID);
int selectAddress(const string& userID);
MYSQL_RES* executeQuery(const string& query, bool fetch = true);
MYSQL_RES* executeSelectQuery(const string& query);
void handleError(const string& message, MYSQL* connection = nullptr);
bool executeUpdateQuery(const string& query);
void searchAndSortProducts();
void generateMonthlyReport(const string& sellerID = " ");
bool applyCouponWithLimit(const string& couponCode, const string& userID);
void viewWalletLogs(const string& userID);
void realTimeStockUpdate(const string& productID);
void processTransactionWithRollback();
bool executeUpdateWithErrorLogging(const string& query);
void logError(const string& error);
void requestRefund(const string& orderID);
void processRefund(const string& refundID);
void adminAnalyticsDashboard();
void displayReceipt(int receiptID);
void displaySalesGraph();
void generateMonthlyReport();
void generateAnnualReport();

// Function Definitions
void connectToDatabase() {
    conn = mysql_init(nullptr);
    if (!conn) {
        cerr << "MySQL Initialization Failed!" << endl;
        exit(EXIT_FAILURE);
    }

    conn = mysql_real_connect(conn, "localhost", "root", "", "shoppay", 3306, nullptr, 0);
    if (!conn) {
        cerr << "Connection failed: " << mysql_error(conn) << endl;
        exit(EXIT_FAILURE);
    }
    else {
        cout << "Connected to the database successfully!\n";
    }
}

bool executeUpdate(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing update: " << mysql_error(conn) << endl;
        return false;
    }
    return true;
}

MYSQL_RES* executeQuery(const string& query, bool fetch) {
    if (mysql_query(conn, query.c_str())) {
        handleError("Query execution failed.", conn);
        return nullptr;
    }
    return fetch ? mysql_store_result(conn) : nullptr;
}

MYSQL_RES* executeSelectQuery(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing query: " << mysql_error(conn) << endl;
        return nullptr;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cerr << "Error storing result: " << mysql_error(conn) << endl;
    }
    return res;
}

bool executeUpdateQuery(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing update: " << mysql_error(conn) << endl;
        return false;
    }
    return true;
}



void handleError(const string& message, MYSQL* connection) {
    cerr << message;
    if (connection) {
        cerr << " Error: " << mysql_error(connection) << endl;
    }
    else {
        cerr << endl;
    }
    exit(EXIT_FAILURE);
}

string fixedPrice(double price) {
    stringstream ss;
    ss << fixed << setprecision(2) << price;
    return ss.str();
}

void clearScreen() {
#ifdef _WIN32
    system("cls"); // Windows
#else
    system("clear"); // Unix/Linux
#endif
}

void displayBanner() {
    cout << "=====================================================" << endl;
    cout << "           WELCOME TO SHOPPAY SYSTEM                 " << endl;
    cout << "   A PLACE WITH PEACE OF MIND TO SHOPPING            " << endl;
    cout << "=====================================================" << endl << endl;
}

void mainInterface() {
    int choice;

    while (true) {
        clearScreen();
        displayBanner();

        cout << "================= MAIN INTERFACE =================\n" << endl;
        cout << "  [1] Register an Account" << endl;
        cout << "  [2] Log In" << endl;
        cout << "  [3] Exit" << endl << endl;
        cout << "==================================================" << endl;
        cout << "      Shop smarter, shop better with ShopPay.     " << endl;
        cout << "==================================================" << endl;
        cout << "Please select an option (1-3): ";
        cin >> choice;

        switch (choice) {
        case 1: // Register
            clearScreen();
            registerUser();
            break;

        case 2: // Login
            clearScreen();
            if (loginUser()) {
                return; // Exit the mainInterface loop
            }
            else {
                cout << "\nInvalid credentials. Please try again.\n";
                cin.ignore();
                cin.get();
            }
            break;

        case 3: // Exit
            cout << "\nThank you for using Shoppay! Goodbye!\n";
            return;

        default:
            cout << "\nInvalid choice. Please select a valid option (1-3).\n";
        }
        cin.ignore();
        cin.get();
    }
}

void registerUser() {
    string temp;
    User newUser;

    cout << "\nEnter Username: ";
    cin >> newUser.username;
    cout << "Enter Email: ";
    cin >> newUser.email;
    cout << "Enter Password: ";
    cin >> newUser.password;
    cin.ignore();
    cout << "Enter Full Name: ";
    getline(cin, newUser.fullName);
    cout << "Enter Phone Number: ";
    getline(cin, temp);
    newUser.phoneNumber = stoi(temp);

    // Default to not approved
    newUser.isApproved = false;

    // Ask for user role
    int roleChoice;
    cout << "Select User Role:" << endl;
    cout << "[1] Admin" << endl;
    cout << "[2] Seller" << endl;
    cout << "[3] Customer" << endl;
    cin >> roleChoice;

    if (roleChoice == 1) {
        string passKey;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clears input buffer
        cout << "Enter the Admin Pass Key: ";
        getline(cin, passKey);

        string correctPassKey = "Shoppay System";
        transform(passKey.begin(), passKey.end(), passKey.begin(), ::tolower);
        transform(correctPassKey.begin(), correctPassKey.end(), correctPassKey.begin(), ::tolower);

        if (passKey == correctPassKey) {
            newUser.UserRole = 2; // Set as admin
        }
        else {
            cout << "Incorrect pass key. You will be registered as a customer." << endl;
            newUser.UserRole = 0; // Default to customer
        }
    }
    else if (roleChoice == 2) {
        newUser.UserRole = 1; // Set as seller
    }
    else {
        newUser.UserRole = 0; // Default to customer
    }

    // Insert into `user` table
    string insertQuery = "INSERT INTO user (Username, Email, Password, FullName, PhoneNumber, UserRole, isApproved) VALUES ('" +
        newUser.username + "', '" + newUser.email + "', '" + newUser.password + "', '" +
        newUser.fullName + "', '" + to_string(newUser.phoneNumber) + "', " +
        to_string(newUser.UserRole) + ", " + to_string(newUser.isApproved) + ")";

    if (!executeUpdate(insertQuery)) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch the last inserted UserID
    string userIDQuery = "SELECT LAST_INSERT_ID()";
    MYSQL_RES* res = executeSelectQuery(userIDQuery);
    if (!res) {
        cout << "Error fetching new UserID: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    string newUserID = row[0];
    mysql_free_result(res);

    // Insert into `seller_details` or `customer_details` based on role
    if (newUser.UserRole == 1) { // Seller
        string storeName, licenseNumber;
        cout << "Enter Store Name: ";
        cin.ignore();
        getline(cin, storeName);
        cout << "Enter Business License Number: ";
        getline(cin, licenseNumber);

        string sellerDetailsQuery = "INSERT INTO seller_details (SellerID, StoreName, BusinessLicenseNumber) VALUES ('" +
            newUserID + "', '" + storeName + "', '" + licenseNumber + "')";
        if (!executeUpdate(sellerDetailsQuery)) {
            cout << "Error adding seller details: " << mysql_error(conn) << endl;
        }
    }
    else if (newUser.UserRole == 0) { // Customer
        string customerDetailsQuery = "INSERT INTO customer_details (CustomerID) VALUES ('" + newUserID + "')";
        if (!executeUpdate(customerDetailsQuery)) {
            cout << "Error adding customer details: " << mysql_error(conn) << endl;
        }
    }

    cout << "Registration successful!\n";
    cout << "Press Enter to return to the main interface...";
    cin.ignore();
    cin.get();
}


// This is Seller Function
void displaySellerDashboard() {
    clearScreen();
    displayBanner();

    // Fetch total products managed by the seller
    string productQuery = "SELECT COUNT(*) FROM product WHERE UserID = '" + glbStr + "'";
    if (mysql_query(conn, productQuery.c_str())) {
        cerr << "Error fetching product count: " << mysql_error(conn) << endl;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);
    int totalProducts = row ? atoi(row[0]) : 0;
    mysql_free_result(res);

    // Fetch total sales for the seller
    string salesQuery = "SELECT SUM(TotalAmount) FROM orders WHERE SellerID = '" + glbStr + "' AND Status = 'Approved'";
    if (mysql_query(conn, salesQuery.c_str())) {
        cerr << "Error fetching sales data: " << mysql_error(conn) << endl;
    }

    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    double totalSales = row ? atof(row[0]) : 0.0;
    mysql_free_result(res);

    // Display dashboard summary
    cout << "================ SELLER DASHBOARD ================" << endl;
    cout << "Total Products: " << totalProducts << endl;
    cout << "Total Sales: $" << fixedPrice(totalSales) << endl;
    cout << "===============================================" << endl;

    displaySellerMenu();
}

void approveSeller() {
    cout << "================= APPROVE SELLER =================" << endl;

    string query =
        "SELECT UserID, Username, Email, Fullname, PhoneNumber "
        "FROM user WHERE UserRole = 1 AND isApproved = 0";
    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row;
    cout << left << setw(10) << "UserID" << setw(20) << "Username"
        << setw(30) << "Email" << setw(20) << "Full Name" << endl;
    cout << string(80, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(20) << row[1]
            << setw(30) << row[2] << setw(20) << row[3] << endl;
    }
    mysql_free_result(res);

    string userID;
    char decision;
    cout << "Enter UserID to approve or decline (or '0' to go back): ";
    cin >> userID;
    if (userID == "0") return;

    cout << "Approve (y/n)? ";
    cin >> decision;

    string update_query;
    if (tolower(decision) == 'y') {
        update_query = "UPDATE user SET isApproved = 1 WHERE UserID = " + userID;
    }
    else {
        update_query = "DELETE FROM user WHERE UserID = " + userID;
    }

    if (executeUpdate(update_query)) {
        cout << (tolower(decision) == 'y' ? "Seller approved!" : "Seller declined and deleted!") << endl;
    }
    else {
        cerr << "Failed to update seller status." << endl;
    }

    cout << "Press Enter to return...";
    cin.ignore();
    cin.get();
}

void displaySellerMenu() {
    int choice;
    while (true) {
        cout << "================= SELLER MENU =================" << endl;
        cout << "[1] Manage My Products" << endl;
        cout << "[2] Browse Products" << endl;
        cout << "[3] Approve Customer Orders" << endl;
        cout << "[4] View Sales Reports" << endl;
        cout << "[5] View Wallet Balance" << endl; // New feature
        cout << "[6] Transaction History" << endl; // New feature
        cout << "[7] Logout" << endl;
        cout << "===============================================" << endl;
        cout << "Please select an option (1-7): ";
        cin >> choice;

        switch (choice) {
        case 1:
            manageMyProducts(); // Manage products
            break;
        case 2:
            browseProducts(); // View promoted products
            break;
        case 3:
            approveCustomerOrders(); // Approve orders
            break;
        case 4:
            viewSalesReport(glbStr); // View seller-specific sales report
            break;
        case 5:
            viewWalletBalance(glbStr); // Call function to view wallet balance
            break;
        case 6:
            viewTransactionHistory(glbStr, "credit"); // Call function for wallet transaction history
            break;
        case 7:
            cout << "\nLogging out...\n";
            return; // Exit seller menu
        default:
            cout << "\nInvalid choice! Please select a valid option.\n";
        }
    }
}


void manageMyProducts() {
    int choice;
    while (true) {
        cout << "================= MANAGE MY PRODUCTS =================" << endl;
        cout << "[1] Add Product" << endl;
        cout << "[2] Update Product" << endl;
        cout << "[3] Delete Product" << endl;
        cout << "[4] View My Products" << endl;
        cout << "[5] Return to Seller Menu" << endl;
        cout << "======================================================" << endl;
        cout << "Please select an option (1-5): ";
        cin >> choice;

        switch (choice) {
        case 1:
            addProductSeller(); // Function to add a new product
            break;
        case 2:
            updateProductSeller(); // Function to update an existing product
            break;
        case 3:
            deleteProductSeller(); // Function to delete a product
            break;
        case 4:
            viewMyProductsSeller(); // Function to view seller's own products
            break;
        case 5:
            return; // Exit to seller menu
        default:
            cout << "\nInvalid choice! Please select a valid option.\n";
            break;
        }
    }
}

// CRUD for Seller

void addProductSeller() {
    string name, description;
    double price;
    int stock;

    cout << "========== ADD PRODUCT ==========" << endl;
    cout << "Enter Product Name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter Product Description: ";
    getline(cin, description);

    cout << "Enter Product Price: $";
    cin >> price;

    cout << "Enter Stock Quantity: ";
    cin >> stock;

    if (price <= 0 || stock <= 0) {
        cout << "Error: Price and Stock Quantity must be greater than zero." << endl;
        return;
    }

    string insert_query =
        "INSERT INTO product (ProductName, Description, Price, StockQuantity, SellerID) VALUES ('" +
        name + "', '" + description + "', " + to_string(price) + ", " + to_string(stock) + ", '" + glbStr + "')";

    if (!executeUpdate(insert_query)) {
        cerr << "Failed to add product. Please try again." << endl;
    }
    else {
        cout << "Product added successfully!" << endl;
    }

    cout << "Press Enter to return to the menu...";
    cin.ignore();
    cin.get();
}

void updateProductSeller() {
    int productId;
    string name, description;
    double price;
    int stock;

    cout << "========== UPDATE PRODUCT ==========" << endl;
    cout << "Enter Product ID to update: ";
    cin >> productId;

    // Verify ownership of the product
    string checkQuery = "SELECT UserID FROM product WHERE ProductID = " + to_string(productId);
    if (mysql_query(conn, checkQuery.c_str())) {
        cout << "Error checking product ownership: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res))) {
        if (to_string(atoi(row[0])) != glbStr) {
            cout << "You do not have permission to update this product." << endl;
            mysql_free_result(res);
            return;
        }
    }
    else {
        cout << "Product not found." << endl;
        mysql_free_result(res);
        return;
    }

    mysql_free_result(res);

    cout << "Enter New Product Name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter New Product Description: ";
    getline(cin, description);

    cout << "Enter New Product Price: $";
    cin >> price;

    cout << "Enter New Stock Quantity: ";
    cin >> stock;

    string update_query = "UPDATE product SET ProductName = '" + name +
        "', Description = '" + description +
        "', Price = " + to_string(price) +
        ", StockQuantity = " + to_string(stock) +
        " WHERE ProductID = " + to_string(productId);

    if (mysql_query(conn, update_query.c_str())) {
        cout << "Error updating product: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product updated successfully!" << endl;
    }

    cout << "Press Enter to return to the menu...";
    cin.ignore();
    cin.get();
}

void deleteProductSeller() {
    int productId;

    cout << "========== DELETE PRODUCT ==========" << endl;
    cout << "Enter Product ID to delete: ";
    cin >> productId;

    // Verify ownership of the product
    string checkQuery = "SELECT UserID FROM product WHERE ProductID = " + to_string(productId);
    if (mysql_query(conn, checkQuery.c_str())) {
        cout << "Error checking product ownership: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res))) {
        if (to_string(atoi(row[0])) != glbStr) {
            cout << "You do not have permission to delete this product." << endl;
            mysql_free_result(res);
            return;
        }
    }
    else {
        cout << "Product not found." << endl;
        mysql_free_result(res);
        return;
    }

    mysql_free_result(res);

    string delete_query = "DELETE FROM product WHERE ProductID = " + to_string(productId);
    if (mysql_query(conn, delete_query.c_str())) {
        cout << "Error deleting product: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product deleted successfully!" << endl;
    }

    cout << "Press Enter to return to the menu...";
    cin.ignore();
    cin.get();
}

void viewMyProductsSeller() {
    int choice;
    bool exitMenu = false;

    while (!exitMenu) {
        clearScreen();
        cout << "========== VIEW MY PRODUCTS ==========\n";

        cout << "[1] View All Products\n";
        cout << "[2] Filter by Category\n";
        cout << "[3] Search by Keyword\n";
        cout << "[4] Exit to Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        string query = "SELECT ProductID, ProductName, Description, Price, StockQuantity "
            "FROM product WHERE UserID = '" + glbStr + "'";

        if (choice == 2) { // Filter by Category
            string category;
            cout << "Enter category name: ";
            cin.ignore();
            getline(cin, category);
            query += " AND Category = '" + category + "'";
        }
        else if (choice == 3) { // Search by Keyword
            string keyword;
            cout << "Enter search keyword: ";
            cin.ignore();
            getline(cin, keyword);
            query += " AND ProductName LIKE '%" + keyword + "%'";
        }
        else if (choice == 4) { // Exit
            exitMenu = true;
            continue;
        }
        else if (choice != 1) {
            cout << "Invalid choice. Please try again.\n";
            cin.ignore();
            cin.get();
            continue;
        }

        // Execute the query
        if (mysql_query(conn, query.c_str())) {
            cerr << "Error fetching products: " << mysql_error(conn) << endl;
            cin.ignore();
            cin.get();
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (!res) {
            cerr << "Error storing result: " << mysql_error(conn) << endl;
            cin.ignore();
            cin.get();
            return;
        }

        MYSQL_ROW row;
        cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
            << setw(10) << "Price" << setw(10) << "Stock" << endl;
        cout << string(100, '-') << endl;

        int count = 0;
        while ((row = mysql_fetch_row(res))) {
            cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
                << setw(10) << "$" + string(row[3]) << setw(10) << row[4] << endl;
            count++;
        }

        mysql_free_result(res);

        if (count == 0) {
            cout << "\nNo products found.\n";
        }

        cout << "\nPress Enter to return...";
        cin.ignore();
        cin.get();
    }
}


// Done CRUD Seller

void approveCustomerOrders() {
    // Logic to fetch and display pending orders for the seller
    // Allow the seller to approve orders
    cout << "================= APPROVE CUSTOMER ORDERS =================" << endl;

    // Query to fetch orders for the seller
    string query = "SELECT OrderID, ProductID, Quantity, TotalAmount FROM orders WHERE SellerID = '" + glbStr + "' AND Status = 'Pending'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Error fetching orders: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << left << setw(10) << "Order ID" << setw(10) << "Product ID" << setw(10) << "Quantity" << setw(10) << "Total" << endl;
    cout << "-----------------------------------------------------" << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << setw(10) << row[0] << setw(10) << row[1] << setw(10) << row[2] << setw(10) << row[3] << endl;
    }

    // Logic to approve selected orders
    int orderId;
    cout << "Enter Order ID to approve: ";
    cin >> orderId;

    string approveQuery = "UPDATE orders SET Status = 'Approved' WHERE OrderID = " + to_string(orderId);
    if (mysql_query(conn, approveQuery.c_str())) {
        cout << "Error approving order: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Order approved successfully!" << endl;
    }

    mysql_free_result(res);
}

void viewSalesReports() {
    // Logic to fetch and display sales reports for the seller
    cout << "================= SALES REPORT =================" << endl;

    // Query to fetch sales data for the seller
    string reportQuery = "SELECT COUNT(ProductID) AS TotalItems, SUM(TotalAmount) AS TotalSales FROM orders WHERE SellerID = '" + glbStr + "' AND Status = 'Approved'";
    if (mysql_query(conn, reportQuery.c_str())) {
        cout << "Error fetching sales report: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res))) {
        int totalItems = atoi(row[0]);
        double totalSales = atof(row[1]);
        double profitPercentage = (totalSales > 0) ? (totalSales * 0.2) : 0; // Assuming 20% profit margin

        cout << "Total Items Sold: " << totalItems << endl;
        cout << "Total Sales: $" << totalSales << endl;
        cout << "Estimated Profit: $" << profitPercentage << endl;
    }
    else {
        cout << "No sales data available." << endl;
    }

    mysql_free_result(res);
}

void contactCustomers() {
    cout << "Contact Customers functionality is under development." << endl;
    // Future implementation will go here
}

void mainMenu() {
    int choice;
    while (true) {
        cout << "---------------------- MENU -------------------------" << endl;
        cout << "[1] Browse Products" << endl;
        cout << "[2] View Cart" << endl;
        cout << "[3] Apply Coupon" << endl;
        cout << "[4] Checkout" << endl;
        cout << "[5] View Wallet Balance" << endl;
        cout << "[6] View Transaction History" << endl;
        cout << "[7] View Receipt" << endl; // Add this option
        cout << "[8] Logout" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "Please select an option (1-8): ";
        cin >> choice;

        switch (choice) {
        case 1:
            browseProducts();
            break;
        case 2:
            viewCart(glbStr);
            break;
        case 3: {
            string couponCode;
            cout << "Enter coupon code: ";
            cin >> couponCode;
            if (applyCoupon(couponCode)) {
                cout << "Coupon applied successfully!" << endl;
            }
            else {
                cout << "Invalid or expired coupon. Please try again." << endl;
            }
            break;
        }
        case 4:
            checkout(glbStr);
            break;
        case 5:
            viewWalletBalance(glbStr);
            break;
        case 6:
            viewTransactionHistory(glbStr, "all");
            break;
        case 7: { // New View Receipt Option
            int receiptID;
            cout << "Enter Receipt ID: ";
            cin >> receiptID;
            displayReceipt(receiptID);
            break;
        }
        case 8:
            cout << "Logging out... Goodbye!" << endl;
            return; // Exit the menu
        default:
            cout << "Invalid option. Please try again." << endl;
        }
        cin.ignore(); // Clear the buffer
        cout << "Press Enter to continue...";
        cin.get();
    }
}



void manageProduct() {
    int choice;

    while (true) {
        clearScreen();
        displayBanner();
        cout << "================= PRODUCT MANAGEMENT =================" << endl;
        cout << "[1] Add Product" << endl;
        cout << "[2] Update Product" << endl;
        cout << "[3] Delete Product" << endl;
        cout << "[4] View All Products" << endl;
        cout << "[5] Search Product" << endl;
        cout << "[6] Return to Admin Menu" << endl;
        cout << "======================================================" << endl;
        cout << "Enter your choice (1-6): ";
        cin >> choice;

        switch (choice) {
        case 1:
            clearScreen();
            addProduct();
            break;
        case 2:
            clearScreen();
            updateProductInterface();
            break;
        case 3:
            clearScreen();
            deleteProductInterface();
            break;
        case 4:
            clearScreen();
            viewProducts();
            break;
        case 5:
            clearScreen();
            searchProductInterface();
            break;
        case 6:
            return; // Exit the product management interface
        default:
            cout << "\nInvalid choice! Please select a valid option (1-6).\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }

        cout << "\nPress Enter to return to the product management menu...";
        cin.ignore();
        cin.get();
    }
}

// ADMIN MENU

void displayAdminMenu() {
    int choice;
    while (true) {
        cout << "================= ADMIN MENU =================" << endl;
        cout << "[1] Manage Products" << endl;
        cout << "[2] View Platform Sales Reports" << endl; // New feature
        cout << "[3] Approve Registered Sellers" << endl;
        cout << "[4] Coupon Management" << endl;
        cout << "[5] Monitor All Wallet Transactions" << endl; // New feature
        cout << "[6] Update Commission Rate" << endl; // New feature
        cout << "[7] Logout" << endl;
        cout << "===============================================" << endl;
        cout << "Please select an option (1-7): ";
        cin >> choice;

        switch (choice) {
        case 1:
            manageProduct(); // Manage products
            break;
        case 2:
            viewSalesReports(); // View platform-wide sales reports
            break;
        case 3:
            approveSeller(); // Approve sellers
            break;
        case 4:
            couponManagement(); // Manage coupons
            break;
        case 5:
            monitorAllWalletTransactions(); // Admin monitors wallet transactions
            break;
        case 6:
            updateCommissionRate(); // Admin updates commission rate
            break;
        case 7:
            cout << "\nLogging out...\n";
            return; // Exit admin menu
        default:
            cout << "\nInvalid choice! Please select a valid option.\n";
        }
    }
}

// COUPON - ADMIN

void couponManagement() {
    int choice;
    while (true) {
        cout << "========== COUPON MANAGEMENT ==========" << endl;
        cout << "[1] Create Coupon" << endl;
        cout << "[2] View All Coupons" << endl;
        cout << "[3] Update Coupon" << endl;
        cout << "[4] Delete Coupon" << endl;
        cout << "[5] Back to Admin Menu" << endl;
        cout << "=======================================" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1: {
            string code, validFrom, validUntil;
            double discountPercentage, minPurchase;
            int usageLimit;

            cout << "Enter Coupon Code: ";
            cin >> code;

            cout << "Enter Discount Percentage (0-100): ";
            cin >> discountPercentage;

            cout << "Enter Valid From (YYYY-MM-DD HH:MM:SS): ";
            cin.ignore();
            getline(cin, validFrom);

            cout << "Enter Valid Until (YYYY-MM-DD HH:MM:SS): ";
            getline(cin, validUntil);

            cout << "Enter Minimum Purchase Amount: ";
            cin >> minPurchase;

            cout << "Enter Usage Limit (-1 for unlimited): ";
            cin >> usageLimit;

            createCoupon(code, discountPercentage, validFrom, validUntil, minPurchase, usageLimit);
            break;
        }
        case 2:
            viewCoupons();
            break;
        case 3: {
            int couponID;
            string validUntil;

            cout << "Enter Coupon ID to update: ";
            cin >> couponID;
            cout << "Enter new Valid Until date (YYYY-MM-DD HH:MM:SS): ";
            cin.ignore();
            getline(cin, validUntil);

            updateCoupon(couponID, validUntil);
            break;
        }
        case 4: {
            int couponID;
            cout << "Enter Coupon ID to delete: ";
            cin >> couponID;
            deleteCoupon(couponID);
            break;
        }
        case 5:
            return;
        default:
            cout << "Invalid choice! Try again.\n";
        }
    }
}

void createCoupon(const string& code, double discountPercentage, const string& validFrom, const string& validUntil, double minPurchase, int usageLimit) {
    string query = "INSERT INTO coupon (Code, DiscountPercentage, ValidFrom, ValidUntil, MinPurchase, UsageLimit) "
        "VALUES ('" + code + "', " + to_string(discountPercentage) + ", '" + validFrom + "', '" +
        validUntil + "', " + to_string(minPurchase) + ", " +
        (usageLimit == -1 ? "NULL" : to_string(usageLimit)) + ")";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error creating coupon: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Coupon created successfully!" << endl;
    }
}

void viewCoupons() {
    string query = "SELECT * FROM coupon";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error fetching coupons: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << left << setw(10) << "ID" << setw(20) << "Code" << setw(15) << "Discount"
        << setw(20) << "Valid From" << setw(20) << "Valid Until" << setw(15) << "Min Purchase"
        << setw(10) << "Usage Limit" << endl;
    cout << string(100, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(15) << row[2]
            << setw(20) << row[3] << setw(20) << row[4] << setw(15) << row[5]
            << setw(10) << (row[6] ? row[6] : "Unlimited") << endl;
    }

    mysql_free_result(res);
}

void updateCoupon(int couponID, const string& validUntil) {
    string query = "UPDATE coupon SET ValidUntil = '" + validUntil + "' WHERE CouponID = " + to_string(couponID);
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error updating coupon: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Coupon updated successfully!" << endl;
    }
}

void deleteCoupon(int couponID) {
    string query = "DELETE FROM coupon WHERE CouponID = " + to_string(couponID);
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error deleting coupon: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Coupon deleted successfully!" << endl;
    }
}


void displayProducts() {
    // Query to fetch all products
    const char* query = "SELECT ProductID, ProductName, Description, Price, StockQuantity FROM product";
    if (mysql_query(conn, query)) {
        cout << "Error fetching products: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr) {
        cout << "Error storing result: " << mysql_error(conn) << endl;
        return;
    }

    cout << "========== PRODUCT LIST ==========\n";
    cout << "ID\tName\t\tDescription\t\tPrice\tStock\n";
    cout << "---------------------------------------------------------\n";

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        cout << row[0] << "\t" << row[1] << "\t" << row[2] << "\t" << row[3] << "\t" << row[4] << endl;
    }

    mysql_free_result(result);
}

void addProduct() {
    viewProducts();

    string name, description;
    double price;
    int stock;

    cout << "========== ADD PRODUCT ==========\n";
    cout << "Enter Product Name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter Product Description: ";
    getline(cin, description);

    cout << "Enter Product Price: $";
    cin >> price;

    cout << "Enter Stock Quantity: ";
    cin >> stock;

    // Step 1: Use the global variable glbStr to get the UserID
    string userID = glbStr; // Assuming glbStr holds the UserID of the logged-in user

    // Step 2: Prepare the insert query for the product table, including UserID
    string insert_query = "INSERT INTO product (ProductName, Description, Price, StockQuantity) VALUES ('" +
        name + "', '" + description + "', " + to_string(price) + ", " + to_string(stock) + ")";

    // Print the query for debugging
    cout << "Query: " << insert_query << endl;

    // Execute the insert query
    if (mysql_query(conn, insert_query.c_str())) {
        cout << "Error adding product: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product added successfully!\n";
    }

    // Back option
    cout << "Press Enter to return to the product management menu...";
    cin.ignore();
    cin.get();
}

void updateProductInterface() {
    viewProducts();

    int productId;
    string name, description;
    double price;
    int stock;

    cout << "========== UPDATE PRODUCT ==========\n";
    cout << "Enter Product ID to update: ";
    cin >> productId;

    cout << "Enter New Product Name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter New Product Description: ";
    getline(cin, description); // Get the new description

    cout << "Enter New Product Price: $";
    cin >> price;

    cout << "Enter New Stock Quantity: ";
    cin >> stock;

    // Prepare the update query
    string query = "UPDATE product SET ProductName = '" + name +
        "', Description = '" + description + "', Price = " + to_string(price) +
        ", StockQuantity = " + to_string(stock) +
        " WHERE ProductID = " + to_string(productId);

    // Execute the update query
    if (mysql_query(conn, query.c_str())) {
        cout << "Error updating product: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product updated successfully!" << endl;
    }

    // Back option
    cout << "Press Enter to return to the product management menu...";
    cin.ignore();
    cin.get();
}

void deleteProductInterface() {
    viewProducts();

    int productId;

    cout << "========== DELETE PRODUCT ==========\n";
    cout << "Enter Product ID to delete: ";
    cin >> productId;

    string query = "DELETE FROM product WHERE ProductID = " + to_string(productId);

    if (mysql_query(conn, query.c_str())) {
        cout << "Error deleting product: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product deleted successfully!" << endl;
    }

    // Back option
    cout << "Press Enter to return to the product management menu...";
    cin.ignore();
    cin.get();
}

void browseProducts() {
    clearScreen();
    displayBanner();
    cout << "====================== BROWSE PRODUCTS ======================" << endl;

    // Fetch and display products from the database
    string query = "SELECT ProductID, ProductName, Price, StockQuantity FROM product";
    if (mysql_query(conn, query.c_str())) {
        cout << "Error fetching products: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    // Header
    cout << left << setw(10) << "ProductID"
        << setw(30) << "ProductName"
        << setw(10) << "Price"
        << setw(10) << "Stock" << endl;
    cout << string(60, '-') << endl; // Separator line

    // Display each product
    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0]                        // ProductID
            << setw(30) << row[1]                                // ProductName
            << setw(10) << "$" + fixedPrice(stod(row[2]))        // Price with $ symbol
            << setw(10) << row[3]                                // Stock
            << endl;
    }
    mysql_free_result(res);

    // Option to add products to cart
    string productID;
    int quantity;
    cout << "\n" << string(60, '-') << endl; // Separator line
    cout << "Enter Product ID to add to cart (or '0' to go back): ";
    cin >> productID;

    if (productID != "0") {
        cout << "Enter Quantity: ";
        cin >> quantity;

        // Validate quantity
        if (quantity <= 0) {
            cout << "Invalid quantity. Please enter a positive number." << endl;
        }
        else {
            // Call function to add item to cart with both quantity and productID
            addItemToCart(quantity, productID); // Ensure both arguments are passed
            cout << "Product ID " << productID << " added to cart with quantity " << quantity << "." << endl;
        }
    }

    // Back option
    cout << "\n" << string(60, '-') << endl; // Separator line
    cout << "Press Enter to return to the main menu...";
    cin.ignore();
    cin.get(); // Wait for user input before returning
}

void viewProducts() {
    cout << "========== VIEW ALL PRODUCTS ==========\n";

    string query = "SELECT * FROM product";

    if (mysql_query(conn, query.c_str())) {
        cout << "Error fetching products: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
        << setw(10) << "Price" << setw(10) << "Stock" << endl;
    cout << string(100, '=') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
            << setw(10) << row[3] << setw(10) << row[4] << endl;
    }

    mysql_free_result(res);

    // Back option
    cout << "Press Enter to return to the product management menu...";
    cin.ignore();
    cin.get();
}

void searchProductInterface() {
    int choice;

    cout << "========== SEARCH PRODUCT ==========\n";
    cout << "[1] Simple Search by Keyword\n";
    cout << "[2] Advanced Search with Filters\n";
    cout << "Select an option (1-2): ";
    cin >> choice;

    switch (choice) {
    case 1: {
        string keyword;
        cout << "Enter keyword: ";
        cin.ignore();
        getline(cin, keyword);

        searchProduct(keyword); // Call the basic search function
        break;
    }
    case 2:
        searchProductWithFilters(); // Call the advanced search function
        break;
    default:
        cout << "Invalid option. Returning to menu.\n";
    }

    cout << "Press Enter to return to the menu...";
    cin.ignore();
    cin.get();
}

void searchProductWithFilters() {
    string keyword, category;
    double minPrice = 0, maxPrice = 0;

    cout << "========== ADVANCED SEARCH ==========\n";
    cout << "Enter keyword (leave blank for all products): ";
    cin.ignore();
    getline(cin, keyword);

    cout << "Enter category (leave blank for all categories): ";
    getline(cin, category);

    cout << "Enter minimum price (default 0): ";
    cin >> minPrice;

    cout << "Enter maximum price (default no limit): ";
    cin >> maxPrice;

    // Build query with filters
    string query = "SELECT * FROM product WHERE 1=1"; // 1=1 simplifies conditional appending
    if (!keyword.empty()) {
        query += " AND ProductName LIKE '%" + keyword + "%'";
    }
    if (!category.empty()) {
        query += " AND CategoryID = '" + category + "'";
    }
    if (maxPrice > 0) {
        query += " AND Price BETWEEN " + to_string(minPrice) + " AND " + to_string(maxPrice);
    }
    else {
        query += " AND Price >= " + to_string(minPrice);
    }

    // Execute query
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error executing advanced search: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cerr << "No results found: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row;
    cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
        << setw(10) << "Price" << setw(10) << "Stock" << endl;
    cout << string(100, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
            << setw(10) << row[3] << setw(10) << row[4] << endl;
    }

    mysql_free_result(res);
    cout << "======================================" << endl;
}


bool loginUser() {
    string username, password;
    cout << "\nEnter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    // Prepare the select query to fetch UserID, isApproved, and UserRole
    string select_query = "SELECT UserID, isApproved, UserRole FROM user WHERE Username = '" + username + "' AND Password = '" + password + "'";

    if (mysql_query(conn, select_query.c_str())) {
        cout << "Error during login: " << mysql_error(conn) << endl;
        return false;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res))) {
        string userID = row[0];      // Fetch UserID
        int isApproved = atoi(row[1]); // Convert isApproved to integer
        int userRole = atoi(row[2]);   // Get the user role

        cout << "Debug: UserID = " << userID << ", isApproved = " << isApproved << ", userRole = " << userRole << endl;

        // Check if the user is a seller and not approved
        if (isApproved == 0 && userRole == 1) {
            cout << "Login failed: Your account is not approved yet." << endl;
            mysql_free_result(res);
            return false; // Prevent login for unapproved sellers
        }
        else {
            cout << "Login successful!" << endl;
            glbStr = userID; // Store the UserID in the global string

            // Redirect based on the user role
            if (userRole == 0) {
                cout << "Welcome, Customer!" << endl;
                mainMenu(); // Redirect to the customer menu
            }
            else if (userRole == 1) {
                cout << "Welcome, Seller!" << endl;
                displaySellerMenu(); // Redirect to the seller menu
            }
            else if (userRole == 2) {
                cout << "Welcome, Admin!" << endl;
                displayAdminMenu(); // Redirect to the admin menu
            }
        }
    }
    else {
        cout << "Login failed: Invalid username or password." << endl;
    }

    mysql_free_result(res);
    cout << "Press Enter to return to the main interface...";
    cin.ignore();
    cin.get();
    return false; // Return false if login failed
}

bool checkAdmin(const string& user) {
    string query = "SELECT UserRole FROM user WHERE Username = '" + user + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Query failed: " << mysql_error(conn) << endl;
        return false; // Default to not admin if query fails
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (res && mysql_num_rows(res) > 0) {
        MYSQL_ROW row = mysql_fetch_row(res);
        return (row[0] && atoi(row[0]) == 1); // Check if the user is an admin (assuming UserRole is stored as 1)
    }
    return false; // Default to not admin if no result
}

void generateCoupon() {
    string code;
    double discount;
    string expirationDate;

    // Collect coupon details from the admin
    cout << "\nEnter Coupon Code: ";
    cin.ignore(); // Ignore leftover newline from previous input
    getline(cin, code);

    cout << "Enter Discount Percentage (e.g., 10 for 10%): ";
    cin >> discount;

    cout << "Enter Expiration Date (YYYY-MM-DD): ";
    cin.ignore(); // Ignore leftover newline from previous input
    getline(cin, expirationDate);

    // Insert coupon into the database
    string insert_query = "INSERT INTO coupons (code, discount, expiration_date) VALUES ('" +
        code + "', '" + to_string(discount) + "', '" + expirationDate + "')";
    const char* q = insert_query.c_str();

    // Execute the query
    if (mysql_query(conn, q)) {
        cout << "Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Coupon generated successfully!" << endl;
    }
}

void viewReports() {
    // Example: Fetch total sales from transactions
    string query = "SELECT SUM(total_amount) FROM transactions"; // Assuming you have a 'transactions' table
    const char* q = query.c_str();

    if (mysql_query(conn, q)) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        double totalSales = stod(row[0]);
        cout << "\nTotal Sales: $" << fixed << setprecision(2) << totalSales << endl;
    }
    else {
        cout << "\nNo transaction data available.\n";
    }

    mysql_free_result(res);

    // Example: Fetch transaction history
    cout << "\nTransaction History:\n";
    query = "SELECT * FROM transactions ORDER BY date DESC"; // Adjust according to your schema
    q = query.c_str();

    if (mysql_query(conn, q)) {
        cout << "Error: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    cout << left << setw(5) << "ID" << setw(20) << "Date" << setw(10) << "Total" << endl;
    cout << "---------------------------------------------" << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << setw(5) << row[0] << setw(20) << row[1] << setw(10) << fixed << setprecision(2) << stod(row[2]) << endl;
    }

    mysql_free_result(res);
}

bool applyCoupon(const string& couponCode) {
    // Query to validate and fetch coupon details
    string query =
        "SELECT DiscountPercentage, MinPurchase, UsageLimit "
        "FROM coupon "
        "WHERE Code = '" + couponCode + "' "
        "AND ValidFrom <= NOW() AND ValidUntil >= NOW() "
        "AND (UsageLimit IS NULL OR UsageLimit > 0)";

    // Execute query
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error fetching coupon details: " << mysql_error(conn) << endl;
        return false;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cerr << "Error processing coupon query result: " << mysql_error(conn) << endl;
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cout << "Invalid or expired coupon code. Please try again." << endl;
        mysql_free_result(res);
        return false;
    }

    // Extract coupon details
    double discountPercentage = atof(row[0]);       // Discount percentage
    double minPurchase = atof(row[1]);             // Minimum purchase amount
    int usageLimit = row[2] ? atoi(row[2]) : -1;   // Usage limit (NULL = unlimited)

    mysql_free_result(res);

    // Check minimum purchase requirement
    if (totalAmount < minPurchase) {
        cout << "Your total amount must be at least $" << minPurchase << " to use this coupon." << endl;
        return false;
    }

    // Apply discount
    double discountAmount = totalAmount * (discountPercentage / 100);
    totalAmount -= discountAmount;

    cout << "Coupon applied successfully! You saved $" << discountAmount << "." << endl;

    // Update usage limit if applicable
    if (usageLimit > 0) {
        string updateQuery =
            "UPDATE coupon SET UsageLimit = UsageLimit - 1 WHERE Code = '" + couponCode + "'";
        if (mysql_query(conn, updateQuery.c_str())) {
            cerr << "Error updating coupon usage: " << mysql_error(conn) << endl;
        }
    }

    return true;
}

//



//

bool isCouponValid(const string& couponCode) {
    string query =
        "SELECT COUNT(*) FROM coupon "
        "WHERE Code = '" + couponCode + "' "
        "AND ValidFrom <= NOW() AND ValidUntil >= NOW() "
        "AND (UsageLimit IS NULL OR UsageLimit > 0)";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Error validating coupon: " << mysql_error(conn) << endl;
        return false;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    bool isValid = row && atoi(row[0]) > 0;
    mysql_free_result(res);

    return isValid;
}

void checkout(const string& userID) {
    // Step 1: Start transaction
    if (mysql_query(conn, "START TRANSACTION")) {
        cerr << "Error starting transaction: " << mysql_error(conn) << endl;
        return;
    }

    // Step 2: Fetch cart items
    string cartQuery = "SELECT c.ProductID, p.ProductName, c.Quantity, p.Price, p.SellerID "
        "FROM cart c "
        "JOIN product p ON c.ProductID = p.ProductID "
        "WHERE c.UserID = ?";
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        cerr << "Failed to initialize statement: " << mysql_error(conn) << endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, cartQuery.c_str(), cartQuery.length())) {
        cerr << "Error preparing statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_BIND param[1];
    memset(param, 0, sizeof(param));

    param[0].buffer_type = MYSQL_TYPE_STRING;
    param[0].buffer = (void*)userID.c_str();
    param[0].buffer_length = userID.length();

    if (mysql_stmt_bind_param(stmt, param)) {
        cerr << "Error binding parameters: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        cerr << "Error executing statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_RES* cartResult = mysql_stmt_result_metadata(stmt);
    if (!cartResult) {
        cerr << "Error fetching cart result metadata: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_ROW cartRow;
    double totalAmount = 0.0;
    vector<tuple<string, int, double, string>> cartItems; // ProductID, Quantity, Price, SellerID

    while ((cartRow = mysql_fetch_row(cartResult))) {
        string productID = cartRow[0];
        int quantity = stoi(cartRow[2]);
        double price = stod(cartRow[3]);
        string sellerID = cartRow[4];

        cartItems.emplace_back(productID, quantity, price, sellerID);
        totalAmount += price * quantity;
    }

    mysql_free_result(cartResult);
    mysql_stmt_close(stmt);

    // Step 3: Fetch wallet balance
    double walletBalance = 0.0;
    string walletQuery = "SELECT Balance FROM wallet WHERE UserID = ?";
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        cerr << "Failed to initialize statement for wallet: " << mysql_error(conn) << endl;
        return;
    }

    if (mysql_stmt_prepare(stmt, walletQuery.c_str(), walletQuery.length())) {
        cerr << "Error preparing wallet statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_bind_param(stmt, param)) {
        cerr << "Error binding wallet parameter: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    if (mysql_stmt_execute(stmt)) {
        cerr << "Error executing wallet statement: " << mysql_stmt_error(stmt) << endl;
        mysql_stmt_close(stmt);
        return;
    }

    MYSQL_RES* walletResult = mysql_stmt_result_metadata(stmt);
    MYSQL_ROW walletRow = mysql_fetch_row(walletResult);
    if (walletRow) {
        walletBalance = stod(walletRow[0]);
    }

    mysql_free_result(walletResult);
    mysql_stmt_close(stmt);

    // Step 4: Validate balance
    if (walletBalance < totalAmount) {
        cerr << "Insufficient wallet balance. Transaction aborted." << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    // Step 5: Deduct balance and update stock
    for (const auto& item : cartItems) {
        const string& productID = get<0>(item);
        int quantity = get<1>(item);
        const string& sellerID = get<3>(item);

        string stockQuery = "UPDATE product SET StockQuantity = StockQuantity - ? WHERE ProductID = ?";
        stmt = mysql_stmt_init(conn);
        if (!stmt) {
            cerr << "Failed to initialize stock statement: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND stockParams[2];
        memset(stockParams, 0, sizeof(stockParams));

        stockParams[0].buffer_type = MYSQL_TYPE_LONG;
        stockParams[0].buffer = (void*)&quantity;
        stockParams[1].buffer_type = MYSQL_TYPE_STRING;
        stockParams[1].buffer = (void*)productID.c_str();
        stockParams[1].buffer_length = productID.length();

        if (mysql_stmt_prepare(stmt, stockQuery.c_str(), stockQuery.length()) ||
            mysql_stmt_bind_param(stmt, stockParams) ||
            mysql_stmt_execute(stmt)) {
            cerr << "Error executing stock update: " << mysql_stmt_error(stmt) << endl;
            mysql_stmt_close(stmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(stmt);

        // Handle seller commission
        double itemTotal = get<2>(item) * quantity;
        double commissionRate = 0.2; // Example: 20%
        double sellerEarnings = itemTotal * (1 - commissionRate);

        string earningsQuery = "UPDATE wallet SET Balance = Balance + ? WHERE UserID = ?";
        stmt = mysql_stmt_init(conn);
        if (!stmt) {
            cerr << "Failed to initialize earnings statement: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            return;
        }

        MYSQL_BIND earningsParams[2];
        memset(earningsParams, 0, sizeof(earningsParams));

        earningsParams[0].buffer_type = MYSQL_TYPE_DOUBLE;
        earningsParams[0].buffer = (void*)&sellerEarnings;
        earningsParams[1].buffer_type = MYSQL_TYPE_STRING;
        earningsParams[1].buffer = (void*)sellerID.c_str();
        earningsParams[1].buffer_length = sellerID.length();

        if (mysql_stmt_prepare(stmt, earningsQuery.c_str(), earningsQuery.length()) ||
            mysql_stmt_bind_param(stmt, earningsParams) ||
            mysql_stmt_execute(stmt)) {
            cerr << "Error updating earnings: " << mysql_stmt_error(stmt) << endl;
            mysql_stmt_close(stmt);
            mysql_query(conn, "ROLLBACK");
            return;
        }

        mysql_stmt_close(stmt);
    }

    // Step 6: Commit transaction
    if (mysql_query(conn, "COMMIT")) {
        cerr << "Error committing transaction: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    cout << "Checkout completed successfully!" << endl;
}



void checkoutTransaction(const string& userID) {
    // Start transaction
    if (mysql_query(conn, "START TRANSACTION")) {
        cerr << "Error starting transaction: " << mysql_error(conn) << endl;
        return;
    }

    // Fetch cart items
    string cartQuery = "SELECT ProductID, Quantity FROM cart WHERE CustomerID = '" + userID + "'";
    if (mysql_query(conn, cartQuery.c_str())) {
        cerr << "Error fetching cart: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cerr << "Error storing cart result: " << mysql_error(conn) << endl;
        mysql_query(conn, "ROLLBACK");
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        string productID = row[0];
        int quantity = stoi(row[1]);

        // Check stock availability
        string stockQuery = "SELECT StockQuantity FROM product WHERE ProductID = '" + productID + "'";
        if (mysql_query(conn, stockQuery.c_str())) {
            cerr << "Error fetching stock: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            mysql_free_result(res);
            return;
        }

        MYSQL_RES* stockRes = mysql_store_result(conn);
        if (!stockRes) {
            cerr << "Error storing stock result: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            mysql_free_result(res);
            return;
        }

        MYSQL_ROW stockRow = mysql_fetch_row(stockRes);
        int stock = stockRow ? stoi(stockRow[0]) : 0;
        mysql_free_result(stockRes);

        if (stock < quantity) {
            cout << "Insufficient stock for ProductID: " << productID << ". Available: " << stock << endl;
            mysql_query(conn, "ROLLBACK");
            mysql_free_result(res);
            return;
        }

        // Update stock
        string updateStockQuery = "UPDATE product SET StockQuantity = StockQuantity - " + to_string(quantity) +
            " WHERE ProductID = '" + productID + "'";
        if (mysql_query(conn, updateStockQuery.c_str())) {
            cerr << "Error updating stock: " << mysql_error(conn) << endl;
            mysql_query(conn, "ROLLBACK");
            mysql_free_result(res);
            return;
        }
    }

    mysql_free_result(res);

    // Commit transaction
    if (mysql_query(conn, "COMMIT")) {
        cerr << "Error committing transaction: " << mysql_error(conn) << endl;
        return;
    }

    cout << "Checkout completed successfully!" << endl;
}

void displayReceipt(const string& orderID, const string& userID) {
    // Fetch customer details
    string customerQuery =
        "SELECT Fullname, Address, Email, PhoneNumber "
        "FROM user WHERE UserID = '" + userID + "' AND UserRole = 0";
    if (mysql_query(conn, customerQuery.c_str())) {
        cerr << "Error fetching customer details: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* customerRes = mysql_store_result(conn);
    MYSQL_ROW customerRow = mysql_fetch_row(customerRes);

    // Fetch transaction details
    string transactionQuery =
        "SELECT TransactionID, TotalPayment, TransactionDate "
        "FROM transaction WHERE OrderID = '" + orderID + "'";
    if (mysql_query(conn, transactionQuery.c_str())) {
        cerr << "Error fetching transaction details: " << mysql_error(conn) << endl;
        mysql_free_result(customerRes);
        return;
    }

    MYSQL_RES* transactionRes = mysql_store_result(conn);
    MYSQL_ROW transactionRow = mysql_fetch_row(transactionRes);

    // Fetch product and seller details
    string receiptQuery =
        "SELECT p.ProductName, p.Price, c.Quantity, "
        "seller.UserID AS SellerID, seller.Fullname AS SellerName, seller.Address AS SellerAddress, "
        "seller.Email AS SellerEmail, sd.StoreName, sd.BusinessLicenseNumber "
        "FROM cart c "
        "JOIN product p ON c.ProductID = p.ProductID "
        "JOIN user seller ON p.SellerID = seller.UserID "
        "LEFT JOIN seller_details sd ON seller.UserID = sd.SellerID "
        "WHERE c.UserID = '" + userID + "' "
        "ORDER BY seller.UserID";

    if (mysql_query(conn, receiptQuery.c_str())) {
        cerr << "Error fetching receipt details: " << mysql_error(conn) << endl;
        mysql_free_result(customerRes);
        mysql_free_result(transactionRes);
        return;
    }

    MYSQL_RES* receiptRes = mysql_store_result(conn);
    if (receiptRes == nullptr || mysql_num_rows(receiptRes) == 0) {
        cerr << "No seller or product data found in the receipt query." << endl;
        mysql_free_result(customerRes);
        mysql_free_result(transactionRes);
        return;
    }

    // Display receipt header
    cout << "------------------- RECEIPT -------------------" << endl;
    cout << "Receipt number: " << transactionRow[0] << endl;
    cout << "Date of purchase: " << transactionRow[2] << endl;
    cout << "-----------------------------------------------" << endl;

    // Display products grouped by seller
    string currentSellerID = "";
    MYSQL_ROW receiptRow;
    while ((receiptRow = mysql_fetch_row(receiptRes))) {
        string sellerID = receiptRow[3];
        if (sellerID != currentSellerID) {
            if (!currentSellerID.empty()) {
                cout << "-----------------------------------------------" << endl;
            }
            cout << "Seller:" << endl;
            cout << "Fullname: " << receiptRow[4] << endl;
            cout << "Address: " << (receiptRow[5] ? receiptRow[5] : "-") << endl;
            cout << "Email: " << (receiptRow[6] ? receiptRow[6] : "-") << endl;
            cout << "Store Name: " << (receiptRow[7] ? receiptRow[7] : "N/A") << endl;
            cout << "Business License: " << (receiptRow[8] ? receiptRow[8] : "N/A") << endl;
            cout << "-----------------------------------------------" << endl;
            currentSellerID = sellerID;
        }

        cout << left << setw(30) << receiptRow[0]                      // Product Name
            << setw(10) << "$" + fixedPrice(stod(receiptRow[1]))     // Price
            << setw(10) << receiptRow[2]                             // Quantity
            << endl;
    }

    cout << "-----------------------------------------------" << endl;
    cout << "Bill By:" << endl;
    cout << "Fullname: " << customerRow[0] << endl;
    cout << "Address: " << customerRow[1] << endl;
    cout << "Email: " << customerRow[2] << endl;
    cout << "Phone: " << customerRow[3] << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "Thank you for your purchase!" << endl;

    mysql_free_result(customerRes);
    mysql_free_result(transactionRes);
    mysql_free_result(receiptRes);
}

void insertProduct(const string& productName, double price, int stock) {
    string insert_query = "INSERT INTO product (ProductName, Price, StockQuantity) VALUES ('" +
        productName + "', " + to_string(price) + ", " + to_string(stock) + ")";
    const char* q = insert_query.c_str();

    if (mysql_query(conn, q)) {
        cout << "Insert Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product inserted successfully!" << endl;
    }
}

void updateProduct(int productId, const string& productName, double price, int stock) {
    string update_query = "UPDATE product SET ProductName = '" + productName +
        "', Price = " + to_string(price) +
        ", StockQuantity = " + to_string(stock) +
        " WHERE ProductID = " + to_string(productId);
    const char* q = update_query.c_str();

    if (mysql_query(conn, q)) {
        cout << "Update Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product updated successfully!" << endl;
    }
}

void deleteProduct(int productId) {
    // Check for dependent rows in the cart table
    string checkCartQuery = "SELECT COUNT(*) FROM cart WHERE ProductID = " + to_string(productId);
    if (mysql_query(conn, checkCartQuery.c_str())) {
        cout << "Error checking cart dependencies: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    int count = atoi(row[0]);
    mysql_free_result(res);

    if (count > 0) {
        cout << "Cannot delete product: It is currently in use in the cart." << endl;
        return;
    }

    // Proceed with product deletion
    string deleteQuery = "DELETE FROM product WHERE ProductID = " + to_string(productId);
    if (mysql_query(conn, deleteQuery.c_str())) {
        cout << "Error deleting product: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Product deleted successfully!" << endl;
    }
}

void searchProduct(const string& keyword) {
    string search_query = "SELECT * FROM product WHERE ProductName LIKE '%" + keyword + "%'";
    const char* q = search_query.c_str();

    if (mysql_query(conn, q)) {
        cout << "Search Error: " << mysql_error(conn) << endl;
    }
    else {
        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row;
        cout << "Search Results:" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "ProductID: " << row[0] << " | ProductName: " << row[1] << " | Price: " << row[2] << " | Stock: " << row[3] << endl;
        }
        mysql_free_result(res);
    }
}

void addItemToCart(int quantity, const string& productID) {
    string stockQuery = "SELECT StockQuantity FROM product WHERE ProductID = '" + productID + "'";
    if (mysql_query(conn, stockQuery.c_str())) {
        cout << "Error fetching stock: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* stockResult = mysql_store_result(conn);
    MYSQL_ROW stockRow;

    if ((stockRow = mysql_fetch_row(stockResult))) {
        int currentStock = stoi(stockRow[0]);
        if (quantity > currentStock) {
            cout << "Not enough stock. Available: " << currentStock << endl;
            mysql_free_result(stockResult);
            return;
        }

        // Insert or update cart
        string query = "INSERT INTO cart (UserID, ProductID, Quantity, TotalPrice) VALUES (" +
            glbStr + ", '" + productID + "', " + to_string(quantity) + ", " +
            "(SELECT Price FROM product WHERE ProductID = '" + productID + "') * " + to_string(quantity) + ") " +
            "ON DUPLICATE KEY UPDATE Quantity = Quantity + " + to_string(quantity) +
            ", TotalPrice = (SELECT Price FROM product WHERE ProductID = '" + productID + "') * Quantity";

        if (mysql_query(conn, query.c_str())) {
            cout << "Add to Cart Error: " << mysql_error(conn) << endl;
        }
        else {
            cout << "Item added successfully!" << endl;

            // Update stock
            int newStock = currentStock - quantity;
            string updateStockQuery = "UPDATE product SET StockQuantity = " + to_string(newStock) +
                " WHERE ProductID = '" + productID + "'";
            if (mysql_query(conn, updateStockQuery.c_str())) {
                cout << "Stock update error: " << mysql_error(conn) << endl;
            }
        }
    }
    else {
        cout << "Product not found." << endl;
    }

    mysql_free_result(stockResult);
}

void updateCartItem(const string& productID, int newQuantity) {
    string stockQuery = "SELECT StockQuantity FROM product WHERE ProductID = '" + productID + "'";
    if (mysql_query(conn, stockQuery.c_str())) {
        cout << "Error fetching stock: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* stockResult = mysql_store_result(conn);
    MYSQL_ROW stockRow;

    if ((stockRow = mysql_fetch_row(stockResult))) {
        int currentStock = stoi(stockRow[0]);
        if (newQuantity > currentStock) {
            cout << "Not enough stock. Available: " << currentStock << endl;
            mysql_free_result(stockResult);
            return;
        }

        // Update cart item quantity
        string query = "UPDATE cart SET Quantity = " + to_string(newQuantity) +
            ", TotalPrice = (SELECT Price FROM product WHERE ProductID = '" + productID + "') * " + to_string(newQuantity) +
            " WHERE ProductID = '" + productID + "' AND UserID = '" + glbStr + "'";
        if (mysql_query(conn, query.c_str())) {
            cout << "Error updating cart: " << mysql_error(conn) << endl;
        }
        else {
            cout << "Cart item updated successfully!" << endl;
        }
    }
    else {
        cout << "Product not found." << endl;
    }

    mysql_free_result(stockResult);
}

void removeCartItem(const string& productID) {
    string query = "DELETE FROM cart WHERE ProductID = '" + productID + "' AND UserID = '" + glbStr + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Error removing item from cart: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Item removed from cart successfully!" << endl;
    }
}

void viewCart(const string& userID) {
    clearScreen();
    displayBanner();

    string query =
        "SELECT c.ProductID, p.ProductName, c.Quantity, p.Price, (c.Quantity * p.Price) AS Total "
        "FROM cart c JOIN product p ON c.ProductID = p.ProductID WHERE c.UserID = '" + userID + "'";
    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row;
    double cartTotal = 0.0;

    cout << "========== YOUR CART ==========" << endl;
    cout << left << setw(10) << "ProductID" << setw(30) << "ProductName"
        << setw(10) << "Quantity" << setw(10) << "Price" << setw(10) << "Total" << endl;
    cout << string(70, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cartTotal += stod(row[4]);
        cout << left << setw(10) << row[0] << setw(30) << row[1]
            << setw(10) << row[2] << setw(10) << "$" + fixedPrice(stod(row[3]))
            << setw(10) << "$" + fixedPrice(stod(row[4])) << endl;
    }
    mysql_free_result(res);

    cout << "Cart Total: $" << fixedPrice(cartTotal) << endl;
    cout << "================================" << endl;
    cout << "[1] Update Quantity\n[2] Remove Item\n[3] Apply Coupon\n[4] Checkout\n[5] Back\n";
    cout << "Select an option: ";

    int choice;
    cin >> choice;
    switch (choice) {
    case 1: {
        string productID;
        int newQuantity;
        cout << "Enter ProductID to update: ";
        cin >> productID;
        cout << "Enter new quantity: ";
        cin >> newQuantity;
        updateCartItem(productID, newQuantity);
        break;
    }
    case 2: {
        string productID;
        cout << "Enter ProductID to remove: ";
        cin >> productID;
        removeCartItem(productID);
        break;
    }
    case 3: {
        string couponCode;
        cout << "Enter coupon code: ";
        cin >> couponCode;
        if (applyCoupon(couponCode)) {
            cout << "Coupon applied!" << endl;
        }
        else {
            cerr << "Failed to apply coupon." << endl;
        }
        break;
    }
    case 4:
        checkout(userID);
        break;
    case 5:
        return;
    default:
        cerr << "Invalid option." << endl;
    }
    viewCart(userID); // Recursive for re-display
}



void placeOrder(const Order& order) {
    string query = "INSERT INTO order (TransactionID, User ID, TotalAmount, OrderStatus) VALUES (0, '" +
        order.userID + "', " + to_string(order.totalAmount) + ", '" + order.status + "')";
    if (mysql_query(conn, query.c_str())) {
        cout << "Place Order Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Order placed successfully!" << endl;
    }
}

void viewOrders(const string& userID) {
    string query = "SELECT * FROM order WHERE UserID = '" + userID + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "View Orders Error: " << mysql_error(conn) << endl;
    }
    else {
        MYSQL_RES* res = mysql_store_result(conn);
        MYSQL_ROW row;
        cout << "Orders:" << endl;
        while ((row = mysql_fetch_row(res))) {
            cout << "OrderID: " << row[0] << ", Total Amount: " << row[2] << ", Status: " << row[3] << endl;
        }
        mysql_free_result(res);
    }
}

void updateOrderStatus(const string& orderID, const string& status) {
    string query = "UPDATE order SET OrderStatus = '" + status + "' WHERE OrderID = '" + orderID + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Update Order Status Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Order status updated successfully!" << endl;
    }
}

void deleteOrder(const string& orderID) {
    string query = "DELETE FROM order WHERE OrderID = '" + orderID + "'";
    if (mysql_query(conn, query.c_str())) {
        cout << "Delete Order Error: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Order deleted successfully!" << endl;
    }
}

void viewWalletBalance(const string& userID) {
    string query = "SELECT Balance FROM wallet WHERE UserID = '" + userID + "'";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error fetching wallet balance: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (row) {
        double balance = stod(row[0]);
        cout << "Your wallet balance is: $" << fixedPrice(balance) << endl;
    }
    else {
        cout << "Wallet not found. Please contact support." << endl;
    }

    mysql_free_result(res);
    cout << "Press Enter to continue...";
    cin.ignore().get();
}

void viewTransactionHistory(const string& userID, const string& transactionType) {
    string query = "SELECT TransactionID, Amount, TransactionType, TransactionDate "
        "FROM transaction WHERE CustomerID = '" + userID + "' ";
    if (transactionType != "all") query += "AND TransactionType = '" + transactionType + "' ";
    query += "ORDER BY TransactionDate DESC";

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row;
    cout << "================ TRANSACTION HISTORY ================" << endl;
    cout << left << setw(15) << "TransactionID" << setw(15) << "Amount"
        << setw(15) << "Type" << setw(30) << "Date" << endl;
    cout << string(60, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << setw(15) << row[0]
            << "$" << setw(14) << fixedPrice(stod(row[1]))
            << setw(15) << row[2]
            << setw(30) << row[3] << endl;
    }
    mysql_free_result(res);
}

void viewSellerWalletBalance(const string& sellerID) {
    string query = "SELECT Balance FROM wallet WHERE UserID = '" + sellerID + "'";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error fetching wallet balance: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res))) {
        cout << "Your Wallet Balance: $" << fixedPrice(stod(row[0])) << endl;
    }
    else {
        cout << "No wallet found for your account. Please contact admin." << endl;
    }

    mysql_free_result(res);
}

void monitorAllWalletTransactions() {
    string query = "SELECT TransactionID, CustomerID, Amount, TransactionType, TransactionDate FROM transaction "
        "ORDER BY TransactionDate DESC";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Error fetching all wallet transactions: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    cout << "================ ALL WALLET TRANSACTIONS ==============" << endl;
    cout << left << setw(15) << "TransactionID" << setw(15) << "CustomerID"
        << setw(15) << "Amount" << setw(15) << "Type" << setw(30) << "Date" << endl;
    cout << string(90, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(15) << row[0] << setw(15) << row[1]
            << setw(15) << "$" + fixedPrice(stod(row[2])) << setw(15) << row[3]
            << setw(30) << row[4] << endl;
    }

    mysql_free_result(res);
    cout << "Press Enter to continue...";
    cin.ignore().get();
}

void updateCommissionRate() {
    double newRate;
    cout << "Enter new commission rate (in %): ";
    cin >> newRate;

    if (newRate < 0 || newRate > 100) {
        cout << "Invalid rate. Please enter a value between 0 and 100." << endl;
        return;
    }

    // Assuming commission rate is stored in a system-wide settings table
    string query = "UPDATE system_settings SET CommissionRate = " + to_string(newRate / 100) + " WHERE SettingName = 'CommissionRate'";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Error updating commission rate: " << mysql_error(conn) << endl;
    }
    else {
        cout << "Commission rate updated successfully!" << endl;
    }

    cout << "Press Enter to continue...";
    cin.ignore().get();
}

void viewSalesReport(const string& sellerID) {
    string query = "SELECT SUM(t.Amount) AS TotalSales, "
        "SUM(t.Amount * 0.1) AS TotalCommission, "
        "SUM(t.Amount * 0.9) AS NetEarnings "
        "FROM transaction t "
        "WHERE t.SellerID = '" + sellerID + "' AND t.TransactionType = 'credit'";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Error fetching sales report for seller: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_RES* res = mysql_store_result(conn);
    MYSQL_ROW row;

    if ((row = mysql_fetch_row(res))) {
        double totalSales = stod(row[0] ? row[0] : "0");
        double totalCommission = stod(row[1] ? row[1] : "0");
        double netEarnings = stod(row[2] ? row[2] : "0");

        cout << "======== SELLER SALES REPORT ========" << endl;
        cout << "Total Sales: $" << fixedPrice(totalSales) << endl;
        cout << "Total Commission (10%): $" << fixedPrice(totalCommission) << endl;
        cout << "Net Earnings: $" << fixedPrice(netEarnings) << endl;
        cout << "=====================================" << endl;
    }
    else {
        cout << "No sales data available for your account." << endl;
    }

    mysql_free_result(res);
}

void addLoyaltyPoints(const string& userID, double totalAmount) {
    int points = static_cast<int>(totalAmount / 10); // Example: 1 point per $10 spent
    string query = "UPDATE customer_details SET LoyaltyPoints = LoyaltyPoints + " + to_string(points) +
        " WHERE CustomerID = '" + userID + "'";
    if (!executeUpdateQuery(query)) {
        cerr << "Error updating loyalty points for userID: " << userID << endl;
    }
    else {
        cout << points << " loyalty points added to your account!" << endl;
    }
}



void addAddress(const string& userID) {
    string line1, line2, city, state, postalCode, country;
    int isDefault;

    cout << "Enter Address Line 1: ";
    cin.ignore();
    getline(cin, line1);

    cout << "Enter Address Line 2 (Optional): ";
    getline(cin, line2);

    cout << "Enter City: ";
    getline(cin, city);

    cout << "Enter State: ";
    getline(cin, state);

    cout << "Enter Postal Code: ";
    getline(cin, postalCode);

    cout << "Enter Country: ";
    getline(cin, country);

    cout << "Is this the default address? (1 = Yes, 0 = No): ";
    cin >> isDefault;

    if (isDefault) {
        // Reset all other addresses to non-default
        string resetDefaultQuery = "UPDATE address SET IsDefault = 0 WHERE UserID = " + userID;
        executeQuery(resetDefaultQuery);
    }

    string query = "INSERT INTO address (UserID, AddressLine1, AddressLine2, City, State, PostalCode, Country, IsDefault) "
        "VALUES ('" + userID + "', '" + line1 + "', '" + line2 + "', '" + city + "', '" + state + "', '" +
        postalCode + "', '" + country + "', " + to_string(isDefault) + ")";

    if (executeQuery(query)) {
        cout << "Address added successfully.\n";
    }
    else {
        cerr << "Failed to add address.\n";
    }
}

void displayAddresses(const string& userID) {
    string query = "SELECT AddressID, AddressLine1, AddressLine2, City, State, PostalCode, Country, IsDefault "
        "FROM address WHERE UserID = " + userID;

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row;
    cout << left << setw(10) << "AddressID" << setw(20) << "Address Line 1" << setw(20) << "Address Line 2"
        << setw(15) << "City" << setw(15) << "State" << setw(10) << "Postal Code" << setw(15) << "Country"
        << setw(10) << "Default" << endl;
    cout << string(100, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(20) << (row[2] ? row[2] : "N/A")
            << setw(15) << row[3] << setw(15) << row[4] << setw(10) << row[5] << setw(15) << row[6]
            << setw(10) << (atoi(row[7]) == 1 ? "Yes" : "No") << endl;
    }
    mysql_free_result(res);
}

int selectAddress(const string& userID) {
    string query = "SELECT AddressID, AddressLine1, City, State, PostalCode, IsDefault "
        "FROM address WHERE UserID = '" + userID + "'";
    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) {
        return -1;
    }

    MYSQL_ROW row;
    vector<int> addressIDs;

    cout << "Select your shipping address:\n";
    while ((row = mysql_fetch_row(res))) {
        int addressID = stoi(row[0]);
        addressIDs.push_back(addressID);
        cout << "[" << addressID << "] " << row[1] << ", " << row[2] << ", " << row[3]
            << " " << row[4] << (stoi(row[5]) ? " (Default)" : "") << endl;
    }
    mysql_free_result(res);

    cout << "Enter AddressID: ";
    int addressID;
    cin >> addressID;
    if (find(addressIDs.begin(), addressIDs.end(), addressID) != addressIDs.end()) {
        return addressID;
    }
    else {
        cout << "Invalid AddressID. Please try again.\n";
        return -1;
    }
}

void searchAndSortProducts() {
    string keyword, sortAttribute;
    double minPrice = 0, maxPrice = 0;

    cout << "Enter keyword (or leave blank for all products): ";
    cin.ignore();
    getline(cin, keyword);

    cout << "Enter minimum price (default 0): ";
    cin >> minPrice;

    cout << "Enter maximum price (default no limit): ";
    cin >> maxPrice;

    cout << "Sort by (price/stock/name): ";
    cin >> sortAttribute;

    string query = "SELECT * FROM product WHERE Price >= " + to_string(minPrice);
    if (maxPrice > 0) query += " AND Price <= " + to_string(maxPrice);
    if (!keyword.empty()) query += " AND ProductName LIKE '%" + keyword + "%'";
    if (!sortAttribute.empty()) query += " ORDER BY " + sortAttribute;

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row;
    cout << left << setw(10) << "ProductID" << setw(20) << "Name" << setw(50) << "Description"
        << setw(10) << "Price" << setw(10) << "Stock" << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(10) << row[0] << setw(20) << row[1] << setw(50) << row[2]
            << setw(10) << row[3] << setw(10) << row[4] << endl;
    }
    mysql_free_result(res);
}

void generateMonthlyReport(const string& sellerID) {
    string query = "SELECT MONTH(TransactionDate) AS Month, SUM(Amount) AS Sales "
        "FROM transaction WHERE TransactionType = 'credit' ";
    if (!sellerID.empty()) query += "AND SellerID = '" + sellerID + "' ";
    query += "GROUP BY MONTH(TransactionDate) ORDER BY MONTH(TransactionDate)";

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row;
    cout << left << setw(10) << "Month" << setw(20) << "Total Sales" << endl;
    cout << string(30, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << setw(10) << row[0] << "$" << row[1] << endl;
    }
    mysql_free_result(res);
}


bool applyCouponWithLimit(const string& couponCode, const string& userID) {
    string query =
        "SELECT CouponID, DiscountPercentage, MinPurchase, UsageLimit "
        "FROM coupon WHERE Code = '" + couponCode + "' AND ValidFrom <= NOW() AND ValidUntil >= NOW()";
    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return false;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cout << "Invalid or expired coupon code.\n";
        mysql_free_result(res);
        return false;
    }

    int couponID = stoi(row[0]);
    double discount = stod(row[1]);
    double minPurchase = stod(row[2]);
    int usageLimit = row[3] ? stoi(row[3]) : -1;

    if (totalAmount < minPurchase) {
        cout << "Minimum purchase not met.\n";
        return false;
    }

    string usageQuery = "SELECT COUNT(*) FROM coupon_usage WHERE UserID = '" + userID +
        "' AND CouponID = " + to_string(couponID);
    MYSQL_RES* usageRes = executeSelectQuery(usageQuery);
    MYSQL_ROW usageRow = mysql_fetch_row(usageRes);

    if (usageLimit > 0 && stoi(usageRow[0]) >= usageLimit) {
        cout << "Coupon usage limit reached.\n";
        mysql_free_result(usageRes);
        return false;
    }

    double discountAmount = totalAmount * (discount / 100);
    totalAmount -= discountAmount;
    cout << "Coupon applied! You saved $" << discountAmount << ".\n";

    string updateUsage = "INSERT INTO coupon_usage (UserID, CouponID) VALUES ('" + userID + "', " + to_string(couponID) + ")";
    executeUpdateQuery(updateUsage);

    return true;
}

void viewWalletLogs(const string& userID) {
    string startDate, endDate, transactionType;

    cout << "Enter start date (YYYY-MM-DD): ";
    cin >> startDate;
    cout << "Enter end date (YYYY-MM-DD): ";
    cin >> endDate;
    cout << "Enter transaction type (credit/debit/all): ";
    cin >> transactionType;

    string query = "SELECT TransactionID, Amount, TransactionType, TransactionDate "
        "FROM wallet_transactions WHERE WalletID = (SELECT WalletID FROM wallet WHERE UserID = '" + userID + "')";
    if (transactionType != "all") query += " AND TransactionType = '" + transactionType + "'";
    query += " AND TransactionDate BETWEEN '" + startDate + "' AND '" + endDate + "' ORDER BY TransactionDate DESC";

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row;
    cout << left << setw(15) << "TransactionID" << setw(15) << "Amount" << setw(15) << "Type" << setw(30) << "Date" << endl;
    cout << string(75, '-') << endl;

    while ((row = mysql_fetch_row(res))) {
        cout << left << setw(15) << row[0] << "$" << setw(15) << row[1] << setw(15) << row[2] << setw(30) << row[3] << endl;
    }
    mysql_free_result(res);
}

void realTimeStockUpdate(const string& productID) {
    string query = "SELECT StockQuantity FROM product WHERE ProductID = '" + productID + "'";
    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cout << "Product not found.\n";
        mysql_free_result(res);
        return;
    }

    int currentStock = stoi(row[0]);
    mysql_free_result(res);

    cout << "Current Stock: " << currentStock << "\n";
    cout << "Enter new stock quantity: ";
    int newStock;
    cin >> newStock;

    if (newStock < 0) {
        cout << "Invalid stock value. Cannot set negative stock.\n";
        return;
    }

    string updateQuery = "UPDATE product SET StockQuantity = " + to_string(newStock) +
        " WHERE ProductID = '" + productID + "'";
    if (executeUpdateQuery(updateQuery)) {
        cout << "Stock updated successfully!\n";
    }
    else {
        cerr << "Failed to update stock.\n";
    }
}

void processTransactionWithRollback() {
    if (mysql_query(conn, "START TRANSACTION")) {
        cerr << "Error starting transaction: " << mysql_error(conn) << "\n";
        return;
    }

    string updateStock = "UPDATE product SET StockQuantity = StockQuantity - 1 WHERE ProductID = '1'";
    if (mysql_query(conn, updateStock.c_str())) {
        cerr << "Error updating stock: " << mysql_error(conn) << "\n";
        mysql_query(conn, "ROLLBACK");
        return;
    }

    string insertOrder = "INSERT INTO orders (CustomerID, TotalAmount) VALUES (1, 100.0)";
    if (mysql_query(conn, insertOrder.c_str())) {
        cerr << "Error inserting order: " << mysql_error(conn) << "\n";
        mysql_query(conn, "ROLLBACK");
        return;
    }

    if (mysql_query(conn, "COMMIT")) {
        cout << "Transaction processed successfully.\n";
    }
    else {
        cerr << "Error committing transaction: " << mysql_error(conn) << "\n";
    }
}

bool executeUpdateWithErrorLogging(const string& query) {
    if (mysql_query(conn, query.c_str())) {
        string errorMessage = mysql_error(conn);
        logError("Query Failed: " + errorMessage);
        return false;
    }
    return true;
}

void logError(const string& error) {
    string query = "INSERT INTO error_logs (ErrorMessage) VALUES ('" + error + "')";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Failed to log error: " << mysql_error(conn) << "\n";
    }
}

void requestRefund(const string& orderID) {
    string query = "SELECT TotalAmount, SellerID FROM orders WHERE OrderID = " + orderID +
        " AND CustomerID = " + glbStr + " AND OrderStatus = 'Delivered'";

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cout << "Invalid order or not eligible for refund.\n";
        mysql_free_result(res);
        return;
    }

    double amount = stod(row[0]);
    string sellerID = row[1];
    mysql_free_result(res);

    string refundQuery = "INSERT INTO refunds (OrderID, CustomerID, SellerID, Amount) VALUES (" + orderID + ", " +
        glbStr + ", " + sellerID + ", " + to_string(amount) + ")";
    if (executeUpdateWithErrorLogging(refundQuery)) {
        cout << "Refund request submitted.\n";
    }
}

void processRefund(const string& refundID) {
    string query =
        "SELECT Amount, CustomerID FROM refunds WHERE RefundID = " + refundID +
        " AND SellerID = " + glbStr + " AND Status = 'Pending'";
    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) return;

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cerr << "Invalid refund ID or refund already processed." << endl;
        mysql_free_result(res);
        return;
    }

    double amount = stod(row[0]);
    string customerID = row[1];
    mysql_free_result(res);

    char decision;
    cout << "Approve refund? (y/n): ";
    cin >> decision;

    if (tolower(decision) == 'y') {
        string refundApproval =
            "UPDATE refunds SET Status = 'Approved' WHERE RefundID = " + refundID;
        string updateCustomer =
            "UPDATE wallet SET Balance = Balance + " + to_string(amount) +
            " WHERE UserID = '" + customerID + "'";
        string updateSeller =
            "UPDATE wallet SET Balance = Balance - " + to_string(amount) +
            " WHERE UserID = '" + glbStr + "'";
        if (executeUpdate(refundApproval) && executeUpdate(updateCustomer) && executeUpdate(updateSeller)) {
            cout << "Refund approved and processed successfully!" << endl;
        }
        else {
            cerr << "Error processing refund." << endl;
        }
    }
    else {
        string refundDecline = "UPDATE refunds SET Status = 'Declined' WHERE RefundID = " + refundID;
        if (executeUpdate(refundDecline)) {
            cout << "Refund declined successfully!" << endl;
        }
        else {
            cerr << "Error declining refund." << endl;
        }
    }
}


void adminAnalyticsDashboard() {
    string salesQuery = "SELECT SUM(TotalAmount) FROM orders WHERE OrderStatus = 'Approved'";
    string refundQuery = "SELECT COUNT(*) FROM refunds WHERE Status = 'Approved'";
    string userQuery = "SELECT COUNT(*) FROM user WHERE UserRole = 0";

    MYSQL_RES* res = executeSelectQuery(salesQuery);
    double totalSales = res ? stod(mysql_fetch_row(res)[0]) : 0;
    mysql_free_result(res);

    res = executeSelectQuery(refundQuery);
    int totalRefunds = res ? atoi(mysql_fetch_row(res)[0]) : 0;
    mysql_free_result(res);

    res = executeSelectQuery(userQuery);
    int totalUsers = res ? atoi(mysql_fetch_row(res)[0]) : 0;
    mysql_free_result(res);

    cout << "Admin Dashboard\n";
    cout << "================\n";
    cout << "Total Sales: $" << totalSales << "\n";
    cout << "Total Refunds: " << totalRefunds << "\n";
    cout << "Total Customers: " << totalUsers << "\n";
}

void displayReceipt(int receiptID) {
    // Query to fetch receipt details
    string receiptQuery =
        "SELECT r.ReceiptID, r.DateOfPurchase, "
        "u.Fullname AS BuyerName, u.Address AS BuyerAddress, u.Email AS BuyerEmail, u.PhoneNumber AS BuyerPhone "
        "FROM receipt r "
        "JOIN user u ON r.UserID = u.UserID "
        "WHERE r.ReceiptID = " + to_string(receiptID);

    MYSQL_RES* res = executeSelectQuery(receiptQuery);
    if (!res) {
        cerr << "Error fetching receipt details. Please try again." << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cerr << "No receipt found with ID: " << receiptID << endl;
        mysql_free_result(res);
        return;
    }

    // Extract receipt header details
    string receiptNumber = row[0];
    string purchaseDate = row[1];
    string buyerName = row[2];
    string buyerAddress = row[3] ? row[3] : "N/A";
    string buyerEmail = row[4] ? row[4] : "N/A";
    string buyerPhone = row[5] ? row[5] : "N/A";

    mysql_free_result(res);

    // Query to fetch seller and product details for this receipt
    string productQuery =
        "SELECT s.Fullname AS SellerName, s.Address AS SellerAddress, s.Email AS SellerEmail, "
        "p.ProductName, p.Price, rp.Quantity, (p.Price * rp.Quantity) AS Subtotal "
        "FROM receipt_product rp "
        "JOIN product p ON rp.ProductID = p.ProductID "
        "JOIN user s ON p.SellerID = s.UserID "
        "WHERE rp.ReceiptID = " + to_string(receiptID) + " "
        "ORDER BY s.UserID";

    res = executeSelectQuery(productQuery);
    if (!res) {
        cerr << "Error fetching product details for receipt. Please try again." << endl;
        return;
    }

    // Display receipt header
    cout << "------------------- RECEIPT -------------------" << endl;
    cout << "Receipt Number: " << receiptNumber << endl;
    cout << "Date of Purchase: " << purchaseDate << endl;

    string currentSellerName = "";
    MYSQL_ROW productRow;
    while ((productRow = mysql_fetch_row(res))) {
        string sellerName = productRow[0];
        string sellerAddress = productRow[1] ? productRow[1] : "N/A";
        string sellerEmail = productRow[2] ? productRow[2] : "N/A";
        string productName = productRow[3];
        double price = stod(productRow[4]);
        int quantity = stoi(productRow[5]);
        double subtotal = stod(productRow[6]);

        // Display seller info once per seller
        if (sellerName != currentSellerName) {
            if (!currentSellerName.empty()) {
                cout << "-----------------------------------------------" << endl;
            }
            cout << "Seller: " << sellerName << endl;
            cout << "Address: " << sellerAddress << endl;
            cout << "Email: " << sellerEmail << endl;
            currentSellerName = sellerName;
        }

        // Display product info
        cout << left << setw(30) << productName
            << setw(10) << "$" + fixedPrice(price)
            << setw(10) << quantity
            << setw(10) << "$" + fixedPrice(subtotal) << endl;
    }

    mysql_free_result(res);

    // Display buyer info
    cout << "-----------------------------------------------" << endl;
    cout << "Bill By:" << endl;
    cout << "Fullname: " << buyerName << endl;
    cout << "Address: " << buyerAddress << endl;
    cout << "Email: " << buyerEmail << endl;
    cout << "Phone: " << buyerPhone << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "Thank you for shopping with us!" << endl;
}

void displaySalesGraph() {
    string query = "SELECT MONTH(TransactionDate) AS Month, SUM(Amount) AS Sales FROM transaction WHERE TransactionType = 'credit' ";

    if (currentUserRole == 1) { // Seller
        query += "AND SellerID = '" + currentUserID + "' ";
    }

    query += "GROUP BY MONTH(TransactionDate) ORDER BY MONTH(TransactionDate)";

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) {
        cerr << "Error fetching sales graph data: " << mysql_error(conn) << endl;
        return;
    }

    // Process and display the graph...
}

void generateMonthlyReport() {
    string query = "SELECT COUNT(TransactionID) AS TotalTransactions, SUM(Amount) AS TotalSales "
        "FROM transaction WHERE TransactionType = 'credit' AND MONTH(TransactionDate) = MONTH(CURDATE()) ";

    // Add condition for sellers
    if (currentUserRole == 1) {
        query += "AND SellerID = '" + currentUserID + "' ";
    }

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) {
        cerr << "Error generating monthly report: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cout << "No data available for this month.\n";
        mysql_free_result(res);
        return;
    }

    int totalTransactions = stoi(row[0]);
    double totalSales = stod(row[1]);
    double netEarnings = (currentUserRole == 1) ? totalSales * 0.9 : totalSales; // Sellers keep 90%

    mysql_free_result(res);

    cout << "\n========== Monthly Report ==========\n";
    cout << "Total Transactions: " << totalTransactions << "\n";
    cout << "Total Sales: $" << fixedPrice(totalSales) << "\n";
    if (currentUserRole == 1) {
        cout << "Net Earnings (after 10% commission): $" << fixedPrice(netEarnings) << "\n";
    }
    cout << "====================================\n";
}

void generateAnnualReport() {
    string query = "SELECT COUNT(TransactionID) AS TotalTransactions, SUM(Amount) AS TotalSales "
        "FROM transaction WHERE TransactionType = 'credit' AND YEAR(TransactionDate) = YEAR(CURDATE()) ";

    // Add condition for sellers
    if (currentUserRole == 1) {
        query += "AND SellerID = '" + currentUserID + "' ";
    }

    MYSQL_RES* res = executeSelectQuery(query);
    if (!res) {
        cerr << "Error generating annual report: " << mysql_error(conn) << endl;
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) {
        cout << "No data available for this year.\n";
        mysql_free_result(res);
        return;
    }

    int totalTransactions = stoi(row[0]);
    double totalSales = stod(row[1]);
    double netEarnings = (currentUserRole == 1) ? totalSales * 0.9 : totalSales; // Sellers keep 90%

    mysql_free_result(res);

    cout << "\n========== Annual Report ==========\n";
    cout << "Total Transactions: " << totalTransactions << "\n";
    cout << "Total Sales: $" << fixedPrice(totalSales) << "\n";
    if (currentUserRole == 1) {
        cout << "Net Earnings (after 10% commission): $" << fixedPrice(netEarnings) << "\n";
    }
    cout << "===================================\n";
}

int main() {
    connectToDatabase(); // Establish database connection

    // Main interface for user registration and login
    mainInterface();

    // Show welcome screen
    displayBanner();

    return 0;
}