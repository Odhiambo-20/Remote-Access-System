#!/bin/bash

# Generate SSL/TLS Certificates for Remote Access System
# This script generates self-signed certificates for testing
# For production, use certificates from a trusted CA

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
echo "SSL/TLS Certificate Generator"
echo "Remote Access System"
echo "============================================"
echo ""

# Check if OpenSSL is installed
if ! command -v openssl &> /dev/null; then
    print_error "OpenSSL is not installed"
    print_info "Install with: sudo apt-get install openssl"
    exit 1
fi

# Certificate directory
CERT_DIR="/etc/remote-access/certs"
BACKUP_DIR="/etc/remote-access/certs/backup"

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    print_error "This script must be run as root"
    print_info "Please run: sudo $0"
    exit 1
fi

# Create certificate directory
print_info "Creating certificate directory..."
mkdir -p "$CERT_DIR"
mkdir -p "$BACKUP_DIR"

# Backup existing certificates if they exist
if [ -f "$CERT_DIR/server.crt" ]; then
    print_info "Backing up existing certificates..."
    BACKUP_DATE=$(date +%Y%m%d_%H%M%S)
    mv "$CERT_DIR"/*.{key,crt,csr,pem} "$BACKUP_DIR/" 2>/dev/null || true
    print_success "Existing certificates backed up to $BACKUP_DIR"
fi

cd "$CERT_DIR"

# Get server information
print_info "Certificate Configuration"
echo ""

read -p "Enter your domain name or IP address [localhost]: " DOMAIN
DOMAIN=${DOMAIN:-localhost}

read -p "Enter your country code (2 letters) [US]: " COUNTRY
COUNTRY=${COUNTRY:-US}

read -p "Enter your state/province [California]: " STATE
STATE=${STATE:-California}

read -p "Enter your city [San Francisco]: " CITY
CITY=${CITY:-"San Francisco"}

read -p "Enter your organization [Remote Access]: " ORG
ORG=${ORG:-"Remote Access"}

read -p "Enter your organizational unit [IT]: " OU
OU=${OU:-IT}

read -p "Enter certificate validity in days [3650]: " DAYS
DAYS=${DAYS:-3650}

echo ""
print_info "Generating certificates for: $DOMAIN"
print_info "Validity: $DAYS days (~$((DAYS/365)) years)"
echo ""

# ============================================================================
# Generate CA (Certificate Authority)
# ============================================================================

print_info "Step 1/5: Generating Certificate Authority (CA)..."

# Generate CA private key
openssl genrsa -out ca.key 4096 2>/dev/null

# Generate CA certificate
openssl req -new -x509 -days "$DAYS" -key ca.key -out ca.crt \
    -subj "/C=$COUNTRY/ST=$STATE/L=$CITY/O=$ORG/OU=$OU/CN=$ORG-CA" \
    2>/dev/null

print_success "CA certificate generated: ca.crt"

# ============================================================================
# Generate Server Certificate
# ============================================================================

print_info "Step 2/5: Generating server private key..."

# Generate server private key
openssl genrsa -out server.key 4096 2>/dev/null

print_success "Server private key generated: server.key"

print_info "Step 3/5: Generating server certificate signing request..."

# Create OpenSSL config for SAN (Subject Alternative Names)
cat > openssl.cnf << EOF
[req]
default_bits = 4096
prompt = no
default_md = sha256
distinguished_name = dn
req_extensions = v3_req

[dn]
C=$COUNTRY
ST=$STATE
L=$CITY
O=$ORG
OU=$OU
CN=$DOMAIN

[v3_req]
keyUsage = critical, digitalSignature, keyEncipherment
extendedKeyUsage = serverAuth, clientAuth
subjectAltName = @alt_names

[alt_names]
DNS.1 = $DOMAIN
DNS.2 = localhost
DNS.3 = *.local
IP.1 = 127.0.0.1
IP.2 = ::1
EOF

# Generate certificate signing request
openssl req -new -key server.key -out server.csr -config openssl.cnf 2>/dev/null

print_success "Server CSR generated: server.csr"

print_info "Step 4/5: Signing server certificate with CA..."

# Sign server certificate with CA
openssl x509 -req -days "$DAYS" -in server.csr -CA ca.crt -CAkey ca.key \
    -CAcreateserial -out server.crt -extensions v3_req -extfile openssl.cnf \
    2>/dev/null

print_success "Server certificate signed: server.crt"

# ============================================================================
# Generate Client Certificates (for mutual TLS)
# ============================================================================

print_info "Step 5/5: Generating client certificates..."

# Generate client private key
openssl genrsa -out client.key 4096 2>/dev/null

# Generate client CSR
openssl req -new -key client.key -out client.csr \
    -subj "/C=$COUNTRY/ST=$STATE/L=$CITY/O=$ORG/OU=$OU/CN=$ORG-Client" \
    2>/dev/null

# Sign client certificate
openssl x509 -req -days "$DAYS" -in client.csr -CA ca.crt -CAkey ca.key \
    -CAcreateserial -out client.crt 2>/dev/null

print_success "Client certificate generated: client.crt"

# ============================================================================
# Generate DH parameters for additional security
# ============================================================================

print_info "Generating Diffie-Hellman parameters (this may take a while)..."
openssl dhparam -out dhparam.pem 2048 2>/dev/null
print_success "DH parameters generated: dhparam.pem"

# ============================================================================
# Create combined PEM files
# ============================================================================

print_info "Creating combined certificate files..."

# Server certificate bundle
cat server.crt ca.crt > server-bundle.crt
print_success "Server bundle created: server-bundle.crt"

# Server PEM (certificate + key)
cat server.crt server.key > server.pem
print_success "Server PEM created: server.pem"

# Client PEM (certificate + key)
cat client.crt client.key > client.pem
print_success "Client PEM created: client.pem"

# ============================================================================
# Set proper permissions
# ============================================================================

print_info "Setting file permissions..."

# Private keys should be readable only by root
chmod 600 *.key *.pem
chmod 644 *.crt *.csr

# Set ownership
chown -R root:root "$CERT_DIR"

print_success "Permissions set correctly"

# ============================================================================
# Verify certificates
# ============================================================================

print_info "Verifying certificates..."

# Verify server certificate
if openssl verify -CAfile ca.crt server.crt &>/dev/null; then
    print_success "Server certificate is valid"
else
    print_error "Server certificate verification failed"
fi

# Verify client certificate
if openssl verify -CAfile ca.crt client.crt &>/dev/null; then
    print_success "Client certificate is valid"
else
    print_error "Client certificate verification failed"
fi

# ============================================================================
# Display certificate information
# ============================================================================

echo ""
echo "============================================"
echo "Certificate Generation Complete!"
echo "============================================"
echo ""

print_info "Certificate Details:"
echo ""

echo "CA Certificate:"
openssl x509 -in ca.crt -noout -subject -issuer -dates

echo ""
echo "Server Certificate:"
openssl x509 -in server.crt -noout -subject -issuer -dates

echo ""
echo "Files generated in: $CERT_DIR"
echo ""
echo "  CA Files:"
echo "    - ca.key         : CA private key"
echo "    - ca.crt         : CA certificate"
echo ""
echo "  Server Files:"
echo "    - server.key     : Server private key"
echo "    - server.crt     : Server certificate"
echo "    - server.csr     : Server certificate signing request"
echo "    - server.pem     : Server combined certificate + key"
echo "    - server-bundle.crt : Server + CA certificate chain"
echo ""
echo "  Client Files:"
echo "    - client.key     : Client private key"
echo "    - client.crt     : Client certificate"
echo "    - client.csr     : Client certificate signing request"
echo "    - client.pem     : Client combined certificate + key"
echo ""
echo "  Additional Files:"
echo "    - dhparam.pem    : Diffie-Hellman parameters"
echo ""

print_info "Important Notes:"
echo ""
echo "1. These are SELF-SIGNED certificates suitable for testing"
echo "2. For production, use certificates from a trusted CA (Let's Encrypt, etc.)"
echo "3. Private keys are stored in $CERT_DIR with restricted permissions"
echo "4. Backup your certificates regularly"
echo "5. Keep your CA private key (ca.key) extremely secure"
echo ""

# ============================================================================
# Update server configuration
# ============================================================================

print_info "Updating server configuration files..."

# Update account server config
ACCOUNT_CONFIG="/etc/remote-access/account_server.conf"
if [ -f "$ACCOUNT_CONFIG" ]; then
    if ! grep -q "ssl_certificate" "$ACCOUNT_CONFIG"; then
        cat >> "$ACCOUNT_CONFIG" << EOF

[ssl]
enabled=true
certificate=$CERT_DIR/server.crt
private_key=$CERT_DIR/server.key
ca_certificate=$CERT_DIR/ca.crt
verify_client=false
EOF
        print_success "Account server config updated"
    fi
fi

# Update relay server config
RELAY_CONFIG="/etc/remote-access/relay_server.conf"
if [ -f "$RELAY_CONFIG" ]; then
    if ! grep -q "ssl_certificate" "$RELAY_CONFIG"; then
        cat >> "$RELAY_CONFIG" << EOF

[ssl]
enabled=true
certificate=$CERT_DIR/server.crt
private_key=$CERT_DIR/server.key
ca_certificate=$CERT_DIR/ca.crt
verify_client=false
EOF
        print_success "Relay server config updated"
    fi
fi

# ============================================================================
# Installation instructions
# ============================================================================

echo ""
print_info "To use these certificates:"
echo ""
echo "1. Restart the servers:"
echo "   sudo systemctl restart remote-access-account-server"
echo "   sudo systemctl restart remote-access-relay-server"
echo ""
echo "2. For clients to trust your CA, copy ca.crt to client machines"
echo ""
echo "3. On Linux clients, install CA certificate:"
echo "   sudo cp $CERT_DIR/ca.crt /usr/local/share/ca-certificates/"
echo "   sudo update-ca-certificates"
echo ""
echo "4. On mobile devices, install ca.crt as a trusted certificate"
echo ""
echo "5. Test connection:"
echo "   openssl s_client -connect $DOMAIN:2809 -CAfile $CERT_DIR/ca.crt"
echo ""

print_success "Certificate generation completed successfully!"