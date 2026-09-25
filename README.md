# 5G Cellular Handover Simulator

[![C++ CI](https://github.com/reddyvaishnavii/5g-cellular-handover-simulator/actions/workflows/ci.yml/badge.svg)](https://github.com/reddyvaishnavii/5g-cellular-handover-simulator/actions/workflows/ci.yml)

A C++17 cellular handover simulator that models UE mobility, signal strength, network conditions, congestion, cell failures, and automatic failover between neighboring cells. The project demonstrates network-aware handover decision logic, reliability, automated testing, and CI.

The project focuses on software-engineering and systems concepts relevant to cellular networking, including decision logic, reliability, automated testing, and continuous integration.

## Features

- UE mobility simulation across multiple cells
- Distance-based signal-strength calculation
- Signal-aware handover decisions
- Cell congestion / utilization checks
- Latency-aware handover decisions
- Packet-loss-aware handover decisions
- Cell failure simulation
- Automatic failover to a healthy neighboring cell
- Cell recovery simulation
- C++ automated tests using CTest
- CSV simulation data export
- Python-based simulation analysis
- Signal-strength visualization
- GitHub Actions CI for automated build and testing

## Architecture

```text
                 User Equipment
                       |
                       v
              +------------------+
              | Mobility Model   |
              +------------------+
                       |
                       v
              +------------------+
              |  Cell Metrics    |
              |------------------|
              | Signal Strength  |
              | Latency          |
              | Packet Loss      |
              | Utilization      |
              | Availability     |
              +------------------+
                       |
                       v
              +------------------+
              | Handover Manager |
              +------------------+
                       |
             +---------+---------+
             |                   |
          Stay/Reject         Handover
                                 |
                                 v
                         New Serving Cell
