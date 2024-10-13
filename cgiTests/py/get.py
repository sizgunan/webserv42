import requests
import time

def main():
    # Change the URL to match your CGI script URL
    url = 'http://localhost:1111/Desktop/WebServ/cgiTests/py/get.py'

    # Make HTTP GET request to itself
    response = requests.get(url)

    # Print the response status code
    print('Response status code:', response.status_code)

    # Print the response content
    print('Response content:')
    print(response.text)

    # Call the main function recursively
    main()

if __name__ == "__main__":
    main()
