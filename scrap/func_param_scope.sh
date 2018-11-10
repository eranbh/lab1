#!/bin/bash

# machine_id=""
func1()
{
    machine_id=223234
}

func2()
{
    func1
    echo $machine_id
}


func2
