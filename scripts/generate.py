import sys
import argparse
import yaml


def main(msgs):
    print("Will process {}".format(msgs))
    with open(msgs, 'r') as f:
        yml = yaml.load(f)
        print(yml)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("msgs", help="Message specification")
    args = parser.parse_args()

    if not args.msgs:
        sys.exit(1)
    main(args.msgs)
