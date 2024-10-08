import requests
import json

github_user = "charbs123-sys"
repo = "House-Prices-Kaggle"
GITHUB_TOKEN = "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"
event_type = "deploy"

url = f'https://api.github.com/repos/{github_user}/{repo}/dispatches'

# I make the request
resp = requests.post(
    url,
    headers={'Authorization': f'token {GITHUB_TOKEN}'},
    data = json.dumps({'event_type': event_type})
    )