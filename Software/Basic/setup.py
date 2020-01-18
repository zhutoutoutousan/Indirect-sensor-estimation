# -*- coding: utf-8 -*-

# Learn more: https://github.com/zhutoutoutousan/NVH_analysis_basic

from setuptools import setup, find_packages


with open('README.rst') as f:
    readme = f.read()

with open('LICENSE') as f:
    license = f.read()

setup(
    name='sample',
    version='0.1.0',
    description='Some basics concerning NVH',
    long_description=readme,
    author='Owen Shao',
    author_email='amazingostian@gmail.com',
    url='https://github.com/zhutoutoutousan/NVH_analysis_basic',
    license=license,
    packages=find_packages(exclude=('tests', 'docs'))
)