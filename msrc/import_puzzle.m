function p = import_puzzle(fname)

    fid = fopen(fname);

    p = [];
    
    s = 1;
    while (~feof(fid))
        line = strtrim(fgetl(fid));
        if length(line) == 0
            continue;
        end
        tmp = [];
        for i = 1:length(line)
            tmp(i) = str2num(line(i));
        end
        p(s,1:length(line)) = tmp;
        s = s + 1;
    end

end
