# https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/iot.html#IoT.Client.create_thing
# https://boto3.amazonaws.com/v1/documentation/api/latest/reference/services/iot/client/create_keys_and_certificate.html#create-keys-and-certificate
# https://boto3.amazonaws.com/v1/documentation/api/latest/search.html?q=attach_thing_principal&check_keywords=yes&area=default

import boto3
import dotenv
import requests
import uuid
import os

# Load environment variables from .env file
dotenv.load_dotenv()
aws_access_key_id = os.environ.get("AWS_ACCESS_KEY_ID")
aws_secret_access_key = os.environ.get("AWS_SECRET_ACCESS_KEY")
aws_region = os.environ.get("AWS_REGION")
if not aws_region or not aws_access_key_id or not aws_secret_access_key:
    raise ValueError("AWS credentials not found in environment variables")

# Create client
client = boto3.client("iot", aws_access_key_id=aws_access_key_id, aws_secret_access_key=aws_secret_access_key, region_name=aws_region)

# Print thing ARN
thing_name = "thing-" + str(uuid.uuid4())[:4]
response = client.create_thing(thingName=thing_name)
thing_arn = response["thingArn"]
print(f"Created thing: {thing_name}")

# Create certificate with keys
cert_response = client.create_keys_and_certificate(setAsActive=True)
certificate_id = cert_response["certificateId"]
certificate_arn = cert_response["certificateArn"]
certificate_pem = cert_response["certificatePem"]
private_key = cert_response["keyPair"]["PrivateKey"]
public_key = cert_response["keyPair"]["PublicKey"]
print(f"Created certificate: {certificate_id}")

# Attach certificate to thing
client.attach_thing_principal(thingName=thing_name, principal=certificate_arn)
print(f"Attached certificate to thing")

# Create a directory for storing certificates
cert_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "certs", thing_name)
os.makedirs(cert_dir, exist_ok=True)

# Write certificate and keys to files
with open(os.path.join(cert_dir, "certificate.pem.crt"), "w") as f:
    f.write(certificate_pem)
with open(os.path.join(cert_dir, "private.pem.key"), "w") as f:
    f.write(private_key)
with open(os.path.join(cert_dir, "public.pem.key"), "w") as f:
    f.write(public_key)

# Download Amazon Root CA 1 certificate
root_ca_url = "https://www.amazontrust.com/repository/AmazonRootCA1.pem"
response = requests.get(root_ca_url)
if response.status_code == 200:
    with open(os.path.join(cert_dir, "AmazonRootCA1.pem"), "w") as f:
        f.write(response.text)
    print(f"Downloaded Amazon Root CA certificate")
else:
    print(f"Failed to download Amazon Root CA certificate. Status code: {response.status_code}")

print(f"Saved certifications to: {cert_dir}")

