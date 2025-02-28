# AWS IoT Core Thing Creator

This readme explains how to use the create_thing.py script to provision IoT devices with AWS IoT Core.

## Prerequisites

1. AWS Account with appropriate permissions
2. Python 3.6 or higher
3. Required Python packages (install using requirements.txt)

## Setup Instructions

### 1. Install Required Python Packages

```bash
pip install -r requirements.txt
```

### 2. Configure AWS Credentials

Rename the provided example environment file:

```bash
.env.example -> .env
```

Edit the .env file and add your AWS credentials:
```
AWS_ACCESS_KEY_ID=your_access_key_id
AWS_SECRET_ACCESS_KEY=your_secret_access_key
AWS_REGION=your_aws_region
```

> **Important:** The AWS credentials must have sufficient permissions to create IoT Things, certificates, and policies.

### 3. Run the Script

Execute the script to create a new IoT Thing:

```bash
python create_thing.py
```

The script will:
- Create a new IoT Thing with a UUID-based name
- Generate a certificate and key pair
- Attach the certificate to the Thing
- Save all certificates and keys to the `certs/{thing-name}` directory

## Output Files

After running the script, you'll find the following files in the `certs/{thing-name}` directory:

- `certificate.pem.crt` - The device certificate
- `private.pem.key` - The device private key
- `public.pem.key` - The device public key

## Next Steps

After creating the Thing:

Copy the newly created files into `data/certs`
   - `certificate.pem.crt` - The device certificate
   - `private.pem.key` - The device private key
   - `AmazonRootCA1.pem` - Amazon Root CA certificate

Then upload them using `Build Filesystem Image` and `Upload Filesystem Image`

## Security Note

- **Never** commit your .env file with real AWS credentials to version control
- Store certificates securely and limit access to authorized personnel only