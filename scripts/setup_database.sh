#!/bin/bash

# Database Setup Script for Remote Access System
# This script initializes the SQLite database with the schema

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_info() {
    echo -e "${YELLOW}[INFO]${NC} $1"
}

echo "============================================"
echo "Database Setup Script"
echo "Remote Access System"
echo "============================================"
echo ""

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    print_error "This script must be run as root"
    print_info "Please run: sudo $0"
    exit 1
fi

# Check if SQLite3 is installed
if ! command -v sqlite3 &> /dev/null; then
    print_error "SQLite3 is not installed"
    print_info "Installing SQLite3..."
    apt-get update && apt-get install -y sqlite3
fi

# Configuration
DB_DIR="/var/lib/remote-access"
DB_FILE="$DB_DIR/accounts.db"
DB_BACKUP_DIR="$DB_DIR/backups"
SCHEMA_FILE="$(dirname $0)/../account-server/sql/schema.sql"

# Create database directory
print_info "Creating database directory..."
mkdir -p "$DB_DIR"
mkdir -p "$DB_BACKUP_DIR"

# Check if database already exists
if [ -f "$DB_FILE" ]; then
    print_info "Database already exists: $DB_FILE"
    
    read -p "Do you want to backup and recreate the database? (y/N): " -n 1 -r
    echo
    
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        # Create backup
        BACKUP_FILE="$DB_BACKUP_DIR/accounts_$(date +%Y%m%d_%H%M%S).db"
        print_info "Creating backup: $BACKUP_FILE"
        cp "$DB_FILE" "$BACKUP_FILE"
        print_success "Backup created"
        
        # Remove old database
        rm "$DB_FILE"
        print_info "Old database removed"
    else
        print_info "Keeping existing database"
        print_info "Verifying database integrity..."
        
        if sqlite3 "$DB_FILE" "PRAGMA integrity_check;" | grep -q "ok"; then
            print_success "Database integrity check passed"
        else
            print_error "Database integrity check failed"
            exit 1
        fi
        
        # Show database statistics
        echo ""
        print_info "Database Statistics:"
        
        USER_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM users;" 2>/dev/null || echo "0")
        PC_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM pcs;" 2>/dev/null || echo "0")
        MOBILE_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM mobile_devices;" 2>/dev/null || echo "0")
        BINDING_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM device_bindings;" 2>/dev/null || echo "0")
        
        echo "  Users: $USER_COUNT"
        echo "  PCs: $PC_COUNT"
        echo "  Mobile Devices: $MOBILE_COUNT"
        echo "  Device Bindings: $BINDING_COUNT"
        
        DB_SIZE=$(du -h "$DB_FILE" | cut -f1)
        echo "  Database Size: $DB_SIZE"
        
        exit 0
    fi
fi

# Check if schema file exists
if [ ! -f "$SCHEMA_FILE" ]; then
    print_error "Schema file not found: $SCHEMA_FILE"
    
    # Try alternative location
    SCHEMA_FILE="./account-server/sql/schema.sql"
    if [ ! -f "$SCHEMA_FILE" ]; then
        print_error "Cannot find schema.sql in expected locations"
        print_info "Please ensure the project structure is intact"
        exit 1
    fi
fi

print_info "Using schema file: $SCHEMA_FILE"

# Create database and apply schema
print_info "Creating database..."
sqlite3 "$DB_FILE" < "$SCHEMA_FILE"

if [ $? -eq 0 ]; then
    print_success "Database created successfully: $DB_FILE"
else
    print_error "Failed to create database"
    exit 1
fi

# Verify database structure
print_info "Verifying database structure..."

EXPECTED_TABLES="users pcs mobile_devices device_bindings sessions audit_log file_shares statistics schema_version"
MISSING_TABLES=""

for table in $EXPECTED_TABLES; do
    if ! sqlite3 "$DB_FILE" "SELECT name FROM sqlite_master WHERE type='table' AND name='$table';" | grep -q "$table"; then
        MISSING_TABLES="$MISSING_TABLES $table"
    fi
done

if [ -n "$MISSING_TABLES" ]; then
    print_error "Missing tables:$MISSING_TABLES"
    exit 1
else
    print_success "All required tables created"
fi

# Verify indexes
print_info "Verifying indexes..."
INDEX_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM sqlite_master WHERE type='index' AND name LIKE 'idx_%';")
print_success "Created $INDEX_COUNT indexes"

# Verify views
VIEW_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM sqlite_master WHERE type='view';")
print_success "Created $VIEW_COUNT views"

# Verify triggers
TRIGGER_COUNT=$(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM sqlite_master WHERE type='trigger';")
print_success "Created $TRIGGER_COUNT triggers"

# Set proper permissions
print_info "Setting file permissions..."
chmod 640 "$DB_FILE"
chown root:root "$DB_FILE"
print_success "Permissions set"

# Optimize database
print_info "Optimizing database..."
sqlite3 "$DB_FILE" "PRAGMA optimize;"
sqlite3 "$DB_FILE" "ANALYZE;"
print_success "Database optimized"

# Display database information
echo ""
echo "============================================"
echo "Database Setup Complete!"
echo "============================================"
echo ""

print_info "Database Information:"
echo ""
echo "  Location: $DB_FILE"
echo "  Size: $(du -h "$DB_FILE" | cut -f1)"
echo ""

# Show schema version
SCHEMA_VERSION=$(sqlite3 "$DB_FILE" "SELECT MAX(version) FROM schema_version;")
echo "  Schema Version: $SCHEMA_VERSION"
echo ""

# List tables
print_info "Database Tables:"
sqlite3 "$DB_FILE" "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;" | sed 's/^/  - /'
echo ""

# Optional: Create test user
read -p "Do you want to create a test user? (y/N): " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo ""
    print_info "Creating test user..."
    
    # Generate test credentials
    TEST_USER="admin"
    TEST_PASS="admin123"
    
    # Hash password (simple SHA256 for demonstration)
    SALT=$(openssl rand -hex 16)
    HASH=$(echo -n "${TEST_PASS}${SALT}" | openssl dgst -sha256 | cut -d' ' -f2)
    
    # Insert test user
    sqlite3 "$DB_FILE" << EOF
INSERT INTO users (username, password_hash, salt, email, full_name) 
VALUES ('$TEST_USER', '$HASH', '$SALT', 'admin@example.com', 'System Administrator');
EOF
    
    if [ $? -eq 0 ]; then
        print_success "Test user created"
        echo ""
        echo "  Username: $TEST_USER"
        echo "  Password: $TEST_PASS"
        echo ""
        print_info "IMPORTANT: Change this password in production!"
    else
        print_error "Failed to create test user"
    fi
fi

# Create database maintenance script
print_info "Creating maintenance script..."

MAINTENANCE_SCRIPT="$DB_DIR/maintenance.sh"

cat > "$MAINTENANCE_SCRIPT" << 'EOF'
#!/bin/bash
# Database Maintenance Script
# Run this periodically to optimize database

DB_FILE="/var/lib/remote-access/accounts.db"
BACKUP_DIR="/var/lib/remote-access/backups"

echo "Starting database maintenance..."

# Create backup
BACKUP_FILE="$BACKUP_DIR/accounts_$(date +%Y%m%d_%H%M%S).db"
echo "Creating backup: $BACKUP_FILE"
cp "$DB_FILE" "$BACKUP_FILE"

# Vacuum database (compact and optimize)
echo "Vacuuming database..."
sqlite3 "$DB_FILE" "VACUUM;"

# Analyze database (update statistics)
echo "Analyzing database..."
sqlite3 "$DB_FILE" "ANALYZE;"

# Clean up old sessions
echo "Cleaning expired sessions..."
sqlite3 "$DB_FILE" "DELETE FROM sessions WHERE expires_at < datetime('now');"

# Clean up old audit logs (keep last 90 days)
echo "Cleaning old audit logs..."
sqlite3 "$DB_FILE" "DELETE FROM audit_log WHERE timestamp < datetime('now', '-90 days');"

# Clean up expired file shares
echo "Cleaning expired file shares..."
sqlite3 "$DB_FILE" "DELETE FROM file_shares WHERE expires_at < datetime('now');"

# Keep only last 30 backups
echo "Cleaning old backups..."
cd "$BACKUP_DIR"
ls -t | tail -n +31 | xargs -r rm --

# Show statistics
echo ""
echo "Database Statistics:"
echo "  Size: $(du -h "$DB_FILE" | cut -f1)"
echo "  Users: $(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM users;")"
echo "  PCs: $(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM pcs;")"
echo "  Online PCs: $(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM pcs WHERE online=1;")"
echo "  Active Sessions: $(sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM sessions WHERE expires_at > datetime('now');")"
echo "  Backups: $(ls -1 | wc -l)"

echo ""
echo "Maintenance complete!"
EOF

chmod +x "$MAINTENANCE_SCRIPT"
print_success "Maintenance script created: $MAINTENANCE_SCRIPT"

# Create cron job for automatic maintenance
read -p "Do you want to setup automatic daily maintenance? (y/N): " -n 1 -r
echo

if [[ $REPLY =~ ^[Yy]$ ]]; then
    print_info "Setting up cron job..."
    
    # Add cron job (runs daily at 3 AM)
    CRON_JOB="0 3 * * * $MAINTENANCE_SCRIPT >> /var/log/remote-access/db_maintenance.log 2>&1"
    
    (crontab -l 2>/dev/null | grep -v "$MAINTENANCE_SCRIPT"; echo "$CRON_JOB") | crontab -
    
    print_success "Cron job created (runs daily at 3:00 AM)"
fi

# Final instructions
echo ""
echo "============================================"
echo "Next Steps"
echo "============================================"
echo ""

print_info "1. Start the account server:"
echo "   sudo systemctl start remote-access-account-server"
echo ""

print_info "2. Check server logs:"
echo "   sudo journalctl -u remote-access-account-server -f"
echo ""

print_info "3. Run maintenance manually:"
echo "   sudo $MAINTENANCE_SCRIPT"
echo ""

print_info "4. View database:"
echo "   sqlite3 $DB_FILE"
echo ""

print_info "5. Backup database:"
echo "   cp $DB_FILE $DB_BACKUP_DIR/accounts_\$(date +%Y%m%d).db"
echo ""

print_success "Database setup completed successfully!"