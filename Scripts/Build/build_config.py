import os
import json
import uuid
import argparse

MARGAVIT_BASE_PATH = os.path.abspath("../..")
MARGAVIT_COMMON_PATH = os.path.join(MARGAVIT_BASE_PATH, "Common")
DEFAULT_CONFIG_JSON_PATH = os.path.join(MARGAVIT_COMMON_PATH, "config.json")
DEFAULT_TEMPLATE_HEADER_PATH = os.path.join(MARGAVIT_COMMON_PATH, "config.template.h")


class JSON(dict):
	def __init__(self, json_dict):
		super(JSON, self).__init__(json_dict)

	def __getattr__(self, index):
		value = self[index]
		return JSON(value) if type(value) is dict else value

	@classmethod
	def from_file(cls, json_path):
		return cls(json.loads(read_file(json_path)))


def parse_args():
	parser = argparse.ArgumentParser(description="Generate a config header file for MargAVit using a given config json")

	parser.add_argument("--config-json-path", dest="config_json_path", default=DEFAULT_CONFIG_JSON_PATH, help="Path to the config json file to use")
	parser.add_argument("--config-header-path", dest="config_header_path", required=True, help="Path to the config header file to generate")
	parser.add_argument("--template_header_path", dest="template_header_path", default=DEFAULT_TEMPLATE_HEADER_PATH, help="Path to the tempalte header file to use")

	return parser.parse_args()


def read_file(file_path):
	with open(file_path, 'r') as file_handle:
		return file_handle.read()


def write_file(file_path, data):
	with open(file_path, 'w') as file_handle:
		file_handle.write(data)


def format_list(lst):
	formatted_list = [format_list(item) if type(item) == list else item for item in lst]
	return "{{{}}}".format(",".join(formatted_list))


def format_timer_names_list(lst):
	return format_list(['L"{}"'.format(timer_name) for timer_name in lst])


def format_hooks_list(lst):
	formatted_list = []

	for dll_path, function_name in lst:
		formatted_list.append('\thooker("{}", {}); \\'.format(dll_path, function_name))

	return "\n".join(formatted_list)


def generate_random_timer_names(count):
	return [str(uuid.uuid4()) for _ in range(count)]


def generate_hooks_list(hooks):
	return [[key, value] for key in hooks.keys() for value in hooks[key]]


def get_hooks_count(hooks):
	return sum(map(len, hooks.values()))


def generate_config_header(config_json, template_header_path):
	template_header_data = read_file(template_header_path)

	return template_header_data.format(
		MinimumAlertTimeS=config_json.MinimumAlertTimeS,
		MaximumAlertTimeS=config_json.MaximumAlertTimeS,
		Timers=format_timer_names_list(generate_random_timer_names(config_json.TimersCount)),
		TimersCount=config_json.TimersCount,
		AlertTimerIndex=config_json.AlertTimerIndex,
		AlertTitle=config_json.AlertTitle,
		AlertDescription=config_json.AlertDescription,
		HookType=config_json.HookType,
		Hooks=format_hooks_list(generate_hooks_list(config_json.Hooks)))

def main():
	args = parse_args()
	config_json = JSON.from_file(args.config_json_path)
	config_header_data = generate_config_header(config_json, args.template_header_path)
	write_file(args.config_header_path, config_header_data)


if __name__ == '__main__':
	main()
