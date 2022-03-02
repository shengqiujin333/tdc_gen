ftcsv = require('ftcsv')
local tdc = require "tdc_gen"

--for zip in ftcsv.parseLine('canshu.csv',',') do
--    print(zip)
--end
local parse = {}
local xsize = 0
local xlen = 0.0
local xtfreq = 0
local xtvol = 0
local xcfreq = 0
local temp_max = 50
local temp_min = 0

local data = {}
for i, line in ftcsv.parseLine("canshu.csv", ",", {bufferSize=50}) do
    local tdc1000_conf = {}
    local tdc7200_conf = {}
    local temp_conf = {}
    for k,v in pairs(line) do
        if (k == 'size')
        then
            xsize = v
        elseif (k == 'len')
        then
            xlen = v
        elseif (k == 'tfreq')
        then
            xtfreq = v
        elseif (k == 'tvol')
        then
            xtvol = v
        elseif (k == 'cfreq')
        then
            xcfreq = v
        else

        end
    end
    tdc1000_conf[1],tdc1000_conf[2],tdc1000_conf[3],
    tdc1000_conf[4],tdc1000_conf[5],tdc1000_conf[6],
    tdc1000_conf[7],tdc1000_conf[8],tdc1000_conf[9],tdc1000_conf[10] = tdc.tdc1000_gen(xlen,xtfreq,xtvol,xcfreq)
    --print(tdc1000_conf[5])
    tdc7200_conf[1],tdc7200_conf[2],tdc7200_conf[3],
    tdc7200_conf[4],tdc7200_conf[5],tdc7200_conf[6],
    tdc7200_conf[7],tdc7200_conf[8],tdc7200_conf[9],tdc7200_conf[10] = tdc.tdc7200_gen()

    temp_conf[1],temp_conf[2],temp_conf[3],
    temp_conf[4],temp_conf[5],temp_conf[6],temp_conf[7] = tdc.tdc_by_temperature(temp_max,xcfreq,xlen,xtfreq,17)

    tdc1000_conf[6] = tdc1000_conf[6] + temp_conf[1]
    tdc1000_conf[7] = tdc1000_conf[7] + temp_conf[2]
    tdc1000_conf[10] = tdc1000_conf[10] + temp_conf[3]

    tdc7200_conf[7] = tdc7200_conf[7] + temp_conf[4]
    tdc7200_conf[8] = tdc7200_conf[8] + temp_conf[5]
    tdc7200_conf[9] = tdc7200_conf[9] + temp_conf[6]
    tdc7200_conf[10] = tdc7200_conf[10] + temp_conf[7]

    data[i] = {}
    data[i].size = xsize
    data[i].tdc1000_config0 = string.format("%x",tdc1000_conf[1])
    data[i].tdc1000_config1 = string.format("%x",tdc1000_conf[2])
    data[i].tdc1000_config2 = string.format("%x",tdc1000_conf[3])
    data[i].tdc1000_config3 = string.format("%x",tdc1000_conf[4])
    data[i].tdc1000_config4 = string.format("%x",tdc1000_conf[5])
    data[i].tdc1000_tof1 = string.format("%x",tdc1000_conf[6])
    data[i].tdc1000_tof0 = string.format("%x",tdc1000_conf[7])
    data[i].tdc1000_error_flags = string.format("%x",tdc1000_conf[8])
    data[i].tdc1000_timeout = string.format("%x",tdc1000_conf[9])
    data[i].tdc1000_clock_rate = string.format("%x",tdc1000_conf[10])

    data[i].tdc7200_config1 = string.format("%x",tdc7200_conf[1])
    data[i].tdc7200_config2 = string.format("%x",tdc7200_conf[2])
    data[i].tdc7200_int_status = string.format("%x",tdc7200_conf[3])
    data[i].tdc7200_int_mask  = string.format("%x",tdc7200_conf[4])
    data[i].tdc7200_coarse_h = string.format("%x",tdc7200_conf[5])
    data[i].tdc7200_coarse_l = string.format("%x",tdc7200_conf[6])
    data[i].tdc7200_clock_ovf_h = string.format("%x",tdc7200_conf[7])
    data[i].tdc7200_clock_ovf_l = string.format("%x",tdc7200_conf[8])
    data[i].tdc7200_clock_mask_h = string.format("%x",tdc7200_conf[9])
    data[i].tdc7200_clock_mask_l = string.format("%x",tdc7200_conf[10])

    --break
end

local fileOutput = ftcsv.encode(data, ",")
local file = assert(io.open("tdc_conf.csv", "w"))
file:write(fileOutput)
file:close()
















